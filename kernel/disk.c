/*
 * ValaOS
 * Copyright (c) 2026 Nehal Vala
 * All rights reserved.
 */

 typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

extern void print(const char* str);

#define ATA_DATA 0x1F0
#define ATA_SECCOUNT 0x1F2
#define ATA_LBA_LOW 0x1F3
#define ATA_LBA_MID 0x1F4
#define ATA_LBA_HIGH 0x1F5
#define ATA_DRIVE 0x1F6
#define ATA_STATUS 0x1F7
#define ATA_COMMAND 0x1F7

#define ATA_CMD_READ 0x20
#define ATA_CMD_WRITE 0x30
#define ATA_CMD_FLUSH 0xE7

#define ATA_SR_BSY 0x80
#define ATA_SR_DRQ 0x08
#define ATA_SR_ERR 0x01
#define ATA_SR_DF 0x20

#define DISK_TEST_SECTOR 100

static uint16_t test_buffer[256] __attribute__((aligned(2)));

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

static void outw(uint16_t port, uint16_t value) {
    __asm__ volatile (
        "outw %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static int ata_wait_not_busy() {
    unsigned int timeout = 1000000;

    while (timeout--) {
        uint8_t status = inb(ATA_STATUS);

        if (!(status & ATA_SR_BSY))
            return !(status & (ATA_SR_ERR | ATA_SR_DF));
    }

    return 0;
}

static int ata_wait_drq() {
    unsigned int timeout = 1000000;

    while (timeout--) {
        uint8_t status = inb(ATA_STATUS);

        if (status & (ATA_SR_ERR | ATA_SR_DF))
            return 0;

        if (status & ATA_SR_DRQ)
            return 1;
    }

    return 0;
}

int disk_read_sector(uint32_t lba, uint16_t* buffer) {
    int i;

    if (!ata_wait_not_busy())
        return 0;

    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECCOUNT, 1);
    outb(ATA_LBA_LOW, lba & 0xFF);
    outb(ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_COMMAND, ATA_CMD_READ);

    if (!ata_wait_not_busy())
        return 0;

    if (!ata_wait_drq())
        return 0;

    for (i = 0; i < 256; i++)
        buffer[i] = inw(ATA_DATA);

    return 1;
}

int disk_write_sector(uint32_t lba, const uint16_t* buffer) {
    int i;

    if (!ata_wait_not_busy())
        return 0;

    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECCOUNT, 1);
    outb(ATA_LBA_LOW, lba & 0xFF);
    outb(ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_COMMAND, ATA_CMD_WRITE);

    if (!ata_wait_not_busy())
        return 0;

    if (!ata_wait_drq())
        return 0;

    for (i = 0; i < 256; i++)
        outw(ATA_DATA, buffer[i]);

    outb(ATA_COMMAND, ATA_CMD_FLUSH);

    if (!ata_wait_not_busy())
        return 0;

    return 1;
}

int disk_test() {
    int i;
    int ok = 1;

    for (i = 0; i < 256; i++)
        test_buffer[i] = (uint16_t)(0xA500u ^ (uint16_t)i);

    print("ATA Disk Test\n\n");
    print("Sector: 100\n");

    if (!disk_write_sector(DISK_TEST_SECTOR, test_buffer)) {
        print("[FAIL] Disk write\n");
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
