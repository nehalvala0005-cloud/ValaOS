/*
 * ValaOS
 * Copyright (c) 2026 Nehal Vala
 * All rights reserved.
 */

 typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

extern void print(const char* str);
extern void print_number(unsigned int number);

#define ATA_DATA 0x1F0
#define ATA_SECCOUNT 0x1F2
#define ATA_LBA_LOW 0x1F3
#define ATA_LBA_MID 0x1F4
#define ATA_LBA_HIGH 0x1F5
#define ATA_DRIVE 0x1F6
#define ATA_STATUS 0x1F7
#define ATA_COMMAND 0x1F7
#define ATA_ALT_STATUS 0x3F6

#define ATA_CMD_READ 0x20
#define ATA_CMD_WRITE 0x30
#define ATA_CMD_FLUSH 0xE7

#define ATA_SR_BSY 0x80
#define ATA_SR_DRQ 0x08
#define ATA_SR_ERR 0x01
#define ATA_SR_DF 0x20

#define DISK_SECTOR_COUNT 2048U
#define DISK_TEST_SECTOR 40U
#define DISK_MARKER_LBA 0U
#define DISK_MARKER_WORD0 0x4156U /* "VA" */
#define DISK_MARKER_WORD1 0x4F4CU /* "LO" */
#define DISK_MARKER_WORD2 0x4453U /* "SD" */
#define DISK_MARKER_WORD3 0x0031U /* "1\0" */

static uint16_t test_buffer[256] __attribute__((aligned(2)));
static int data_disk_ready;
static uint8_t ata_last_status;
static unsigned int ata_last_stage;

static uint8_t inb(uint16_t port) {
    uint8_t value;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}

static void outb(uint16_t port, uint8_t value) {
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static uint16_t inw(uint16_t port) {
    uint16_t value;

    __asm__ volatile (
        "inw %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}

/* ATA requires roughly 400 ns after drive selection and command writes. */
static void ata_io_delay(void) {
    (void)inb(ATA_ALT_STATUS);
    (void)inb(ATA_ALT_STATUS);
    (void)inb(ATA_ALT_STATUS);
    (void)inb(ATA_ALT_STATUS);
}

static void outw(uint16_t port, uint16_t value) {
    __asm__ volatile (
        "outw %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static int ata_wait_not_busy() {
    unsigned int timeout = 10000000;

    while (timeout--) {
        uint8_t status = inb(ATA_STATUS);
        ata_last_status = status;

        if (status == 0 || status == 0xFF)
            return 0;
        if (!(status & ATA_SR_BSY))
            return !(status & (ATA_SR_ERR | ATA_SR_DF));
    }

    return 0;
}

static int ata_wait_ready() {
    unsigned int timeout = 10000000;

    while (timeout--) {
        uint8_t status = inb(ATA_STATUS);
        ata_last_status = status;

        if (status == 0 || status == 0xFF)
            return 0;
        if (status & (ATA_SR_ERR | ATA_SR_DF))
            return 0;
        if (!(status & (ATA_SR_BSY | ATA_SR_DRQ)))
            return 1;
    }
    return 0;
}

static int ata_wait_drq() {
    unsigned int timeout = 10000000;

    while (timeout--) {
        uint8_t status = inb(ATA_STATUS);
        ata_last_status = status;

        if (status == 0 || status == 0xFF)
            return 0;
        if (status & (ATA_SR_ERR | ATA_SR_DF))
            return 0;

        if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ))
            return 1;
    }

    return 0;
}

static int ata_read_sector(uint32_t lba, uint16_t* buffer) {
    int i;

    if (!ata_wait_ready())
        return 0;

    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    ata_io_delay();
    if (!ata_wait_ready())
        return 0;
    outb(ATA_SECCOUNT, 1);
    outb(ATA_LBA_LOW, lba & 0xFF);
    outb(ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_COMMAND, ATA_CMD_READ);
    ata_io_delay();

    if (!ata_wait_not_busy())
        return 0;

    if (!ata_wait_drq())
        return 0;

    for (i = 0; i < 256; i++)
        buffer[i] = inw(ATA_DATA);

    return ata_wait_ready();
}

static int ata_write_sector(uint32_t lba, const uint16_t* buffer) {
    int i;

    ata_last_stage = 1;
    if (!ata_wait_ready())
        return 0;

    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    ata_io_delay();
    ata_last_stage = 2;
    if (!ata_wait_ready())
        return 0;
    outb(ATA_SECCOUNT, 1);
    outb(ATA_LBA_LOW, lba & 0xFF);
    outb(ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_COMMAND, ATA_CMD_WRITE);
    ata_io_delay();

    ata_last_stage = 3;
    if (!ata_wait_not_busy())
        return 0;

    ata_last_stage = 4;
    if (!ata_wait_drq())
        return 0;

    for (i = 0; i < 256; i++)
        outw(ATA_DATA, buffer[i]);

    ata_last_stage = 5;
    return ata_wait_ready();
}

/*
 * The kernel uses the primary-master ATA LBA28 device. To avoid writing to
 * arbitrary attached disks, writes are enabled only when sector zero has the
 * marker emitted by tools/create_vfs_image.py. The supported test image is
 * 2048 sectors (1 MiB); the driver deliberately exposes no sectors beyond it.
 */
int disk_init(void) {
    uint16_t marker[256];

    data_disk_ready = 0;
    outb(ATA_DRIVE, 0xE0);
    ata_io_delay();
    if (!ata_wait_ready())
        return 0;
    if (!ata_read_sector(DISK_MARKER_LBA, marker))
        return 0;
    if (marker[0] != DISK_MARKER_WORD0 ||
        marker[1] != DISK_MARKER_WORD1 ||
        marker[2] != DISK_MARKER_WORD2 ||
        marker[3] != DISK_MARKER_WORD3)
        return 0;

    data_disk_ready = 1;
    return 1;
}

int disk_is_ready(void) {
    return data_disk_ready;
}

uint32_t disk_sector_count(void) {
    return data_disk_ready ? DISK_SECTOR_COUNT : 0;
}

int disk_read_sector(uint32_t lba, uint16_t* buffer) {
    if (!buffer || lba >= DISK_SECTOR_COUNT || !data_disk_ready)
        return 0;
    return ata_read_sector(lba, buffer);
}

int disk_write_sector(uint32_t lba, const uint16_t* buffer) {
    if (!buffer || lba == DISK_MARKER_LBA ||
        lba >= DISK_SECTOR_COUNT || !data_disk_ready)
        return 0;
    return ata_write_sector(lba, buffer);
}

int disk_flush(void) {
    if (!data_disk_ready || !ata_wait_ready())
        return 0;
    outb(ATA_DRIVE, 0xE0);
    ata_io_delay();
    if (!ata_wait_ready())
        return 0;
    outb(ATA_COMMAND, ATA_CMD_FLUSH);
    ata_io_delay();
    if (!ata_wait_not_busy())
        return 0;
    return ata_wait_ready();
}

int disk_test() {
    int i;
    int ok = 1;

    for (i = 0; i < 256; i++)
        test_buffer[i] = (uint16_t)(0xA500u ^ (uint16_t)i);

    if (!disk_is_ready()) {
        print("[ ERROR ] Dedicated ValaOS data disk is not available\n");
        return 0;
    }

    print("ATA Disk Test (dedicated data disk)\n\n");
    print("Sector: ");
    print_number(DISK_TEST_SECTOR);
    print("\n");

    if (!disk_write_sector(DISK_TEST_SECTOR, test_buffer)) {
        print("[FAIL] Disk write stage ");
        print_number(ata_last_stage);
        print(" status ");
        print_number(ata_last_status);
        print("\n");
        return 0;
    }

    if (!disk_flush()) {
        print("[FAIL] Disk flush\n");
        return 0;
    }

    print("[ OK ] Sector write\n");

    for (i = 0; i < 256; i++)
        test_buffer[i] = 0;

    if (!disk_read_sector(DISK_TEST_SECTOR, test_buffer)) {
        print("[FAIL] Disk read\n");
        return 0;
    }

    print("[ OK ] Sector read\n");

    for (i = 0; i < 256; i++) {
        if (test_buffer[i] != (uint16_t)(0xA500u ^ (uint16_t)i)) {
            ok = 0;
            break;
        }
    }

    if (ok)
        print("[ OK ] Read/write data verified\n");
    else
        print("[FAIL] Data verification\n");

    if (ok)
        print("\nATA disk test passed.\n");
    else
        print("\nATA disk test failed.\n");

    return ok;
}
