/*
 * ValaOS
 * Copyright (c) 2026 Nehal Vala
 * All rights reserved.
 */

 typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define VFS_MAX_ENTRIES 16
#define VFS_NAME_SIZE 32
#define VFS_DATA_SIZE 256

#define VFS_HEADER_SECTOR 1
#define VFS_DATA_START_SECTOR 2
#define VFS_DATA_SECTORS 10
#define VFS_ENTRY_SIZE 300
#define VFS_STORAGE_BYTES (VFS_MAX_ENTRIES * VFS_ENTRY_SIZE)

#define VFS_MAGIC 0x56414C41
#define VFS_VERSION 1

struct vfs_entry {
    int used;
    int is_dir;
    int parent;
    char name[VFS_NAME_SIZE];
    char data[VFS_DATA_SIZE];
};

static struct vfs_entry entries[VFS_MAX_ENTRIES];
static int current_dir = 0;
static uint8_t storage[VFS_DATA_SECTORS * 512];

extern void print(const char* str);
extern int disk_read_sector(uint32_t lba, uint16_t* buffer);
extern int disk_write_sector(uint32_t lba, const uint16_t* buffer);

static int vfs_compare(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b)
            return 0;

        a++;
        b++;
    }

    return *a == *b;
}

static void vfs_copy(char* dest, const char* src, int limit) {
    int i = 0;

    while (src[i] && i < limit - 1) {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
}

static int vfs_find_child(int parent, const char* name) {
    int i;

    for (i = 0; i < VFS_MAX_ENTRIES; i++) {
        if (entries[i].used &&
            entries[i].parent == parent &&
            vfs_compare(entries[i].name, name))
            return i;
    }

    return -1;
}

static int vfs_find_free() {
    int i;

    for (i = 0; i < VFS_MAX_ENTRIES; i++) {
        if (!entries[i].used)
            return i;
    }

    return -1;
}

static void vfs_clear_storage() {
    int i;

    for (i = 0; i < VFS_DATA_SECTORS * 512; i++)
        storage[i] = 0;
}

static void vfs_put_u32(uint8_t* buffer, int* offset, uint32_t value) {
    buffer[*offset + 0] = (uint8_t)(value & 0xFF);
    buffer[*offset + 1] = (uint8_t)((value >> 8) & 0xFF);
    buffer[*offset + 2] = (uint8_t)((value >> 16) & 0xFF);
    buffer[*offset + 3] = (uint8_t)((value >> 24) & 0xFF);

    *offset += 4;
}

static uint32_t vfs_get_u32(const uint8_t* buffer, int* offset) {
    uint32_t value;

    value = (uint32_t)buffer[*offset + 0];
    value |= (uint32_t)buffer[*offset + 1] << 8;
    value |= (uint32_t)buffer[*offset + 2] << 16;
    value |= (uint32_t)buffer[*offset + 3] << 24;

    *offset += 4;

    return value;
}

static void vfs_serialize() {
    int i;
    int j;
    int offset;

    vfs_clear_storage();
    offset = 0;

    for (i = 0; i < VFS_MAX_ENTRIES; i++) {
        vfs_put_u32(
            storage,
            &offset,
            (uint32_t)entries[i].used
        );

        vfs_put_u32(
            storage,
            &offset,
            (uint32_t)entries[i].is_dir
        );

        vfs_put_u32(
            storage,
            &offset,
            (uint32_t)entries[i].parent
        );

        for (j = 0; j < VFS_NAME_SIZE; j++)
            storage[offset++] = (uint8_t)entries[i].name[j];

        for (j = 0; j < VFS_DATA_SIZE; j++)
            storage[offset++] = (uint8_t)entries[i].data[j];
    }
}

static void vfs_deserialize() {
    int i;
    int j;
    int offset;

    offset = 0;

    for (i = 0; i < VFS_MAX_ENTRIES; i++) {
        entries[i].used = (int)vfs_get_u32(storage, &offset);
        entries[i].is_dir = (int)vfs_get_u32(storage, &offset);
        entries[i].parent = (int)vfs_get_u32(storage, &offset);

        for (j = 0; j < VFS_NAME_SIZE; j++)
            entries[i].name[j] = (char)storage[offset++];

        for (j = 0; j < VFS_DATA_SIZE; j++)
            entries[i].data[j] = (char)storage[offset++];

        entries[i].name[VFS_NAME_SIZE - 1] = '\0';
        entries[i].data[VFS_DATA_SIZE - 1] = '\0';
    }
}

static int vfs_read_sector(uint32_t lba, uint8_t* buffer) {
    uint16_t words[256];
    int i;

    if (!disk_read_sector(lba, words))
        return 0;

    for (i = 0; i < 256; i++) {
        buffer[i * 2] = (uint8_t)(words[i] & 0xFF);
        buffer[i * 2 + 1] = (uint8_t)(words[i] >> 8);
    }

    return 1;
}

static int vfs_write_sector(uint32_t lba, const uint8_t* buffer) {
    uint16_t words[256];
    int i;

    for (i = 0; i < 256; i++)
        words[i] = (uint16_t)buffer[i * 2] |
                   ((uint16_t)buffer[i * 2 + 1] << 8);

    return disk_write_sector(lba, words);
}

static int vfs_save() {
    uint8_t header[512];
    int header_offset = 0;
    int sector;
    int offset;

    for (offset = 0; offset < 512; offset++)
        header[offset] = 0;

    vfs_put_u32(header, &header_offset, VFS_MAGIC);
    vfs_put_u32(header, &header_offset, VFS_VERSION);
    vfs_put_u32(header, &header_offset, VFS_MAX_ENTRIES);
    vfs_put_u32(header, &header_offset, VFS_ENTRY_SIZE);
    vfs_put_u32(header, &header_offset, VFS_DATA_SECTORS);

    vfs_serialize();

    if (!vfs_write_sector(VFS_HEADER_SECTOR, header))
        return 0;

    for (sector = 0; sector < VFS_DATA_SECTORS; sector++) {
        if (!vfs_write_sector(
                (uint32_t)(VFS_DATA_START_SECTOR + sector),
                &storage[sector * 512]))
            return 0;
    }

    return 1;
}

static int vfs_load() {
    uint8_t header[512];
    int offset = 0;
    uint32_t magic;
    uint32_t version;
    uint32_t entries_count;
    uint32_t entry_size;
    uint32_t sector_count;
    int sector;

    if (!vfs_read_sector(VFS_HEADER_SECTOR, header))
        return 0;

    magic = vfs_get_u32(header, &offset);
    version = vfs_get_u32(header, &offset);
    entries_count = vfs_get_u32(header, &offset);
    entry_size = vfs_get_u32(header, &offset);
    sector_count = vfs_get_u32(header, &offset);

    if (magic != VFS_MAGIC)
        return 0;

    if (version != VFS_VERSION)
        return 0;

    if (entries_count != VFS_MAX_ENTRIES)
        return 0;

    if (entry_size != VFS_ENTRY_SIZE)
        return 0;

    if (sector_count != VFS_DATA_SECTORS)
        return 0;

    for (sector = 0; sector < VFS_DATA_SECTORS; sector++) {
        if (!vfs_read_sector(
                (uint32_t)(VFS_DATA_START_SECTOR + sector),
                &storage[sector * 512]))
            return 0;
    }

    vfs_deserialize();

    if (!entries[0].used || !entries[0].is_dir)
        return 0;

    current_dir = 0;

    return 1;
}

static void vfs_create_default() {
    int i;

    for (i = 0; i < VFS_MAX_ENTRIES; i++) {
        entries[i].used = 0;
        entries[i].is_dir = 0;
        entries[i].parent = -1;
        entries[i].name[0] = '\0';
        entries[i].data[0] = '\0';
    }

    entries[0].used = 1;
    entries[0].is_dir = 1;
    entries[0].parent = -1;
    vfs_copy(entries[0].name, "/", VFS_NAME_SIZE);

    entries[1].used = 1;
    entries[1].is_dir = 0;
    entries[1].parent = 0;
    vfs_copy(entries[1].name, "readme.txt", VFS_NAME_SIZE);
    vfs_copy(
        entries[1].data,
        "Welcome to ValaOS!\nThis is the ValaOS virtual file system.\n",
        VFS_DATA_SIZE
    );

    entries[2].used = 1;
    entries[2].is_dir = 0;
    entries[2].parent = 0;
    vfs_copy(entries[2].name, "hello.txt", VFS_NAME_SIZE);
    vfs_copy(
        entries[2].data,
        "Hello from ValaOS.\n",
        VFS_DATA_SIZE
    );

    entries[3].used = 1;
    entries[3].is_dir = 0;
    entries[3].parent = 0;
    vfs_copy(entries[3].name, "system.txt", VFS_NAME_SIZE);
    vfs_copy(
        entries[3].data,
        "ValaOS 2.0\nArchitecture: x86\nFilesystem: VFS\n",
        VFS_DATA_SIZE
    );

    current_dir = 0;
}

void vfs_init() {
    if (vfs_load())
        return;

    vfs_create_default();
    vfs_save();
}

void vfs_list() {
    int i;

    print("NAME\n");

    for (i = 0; i < VFS_MAX_ENTRIES; i++) {
        if (entries[i].used && entries[i].parent == current_dir) {
            print(entries[i].name);

            if (entries[i].is_dir)
                print("/");

            print("\n");
        }
    }
}

int vfs_cat(const char* name) {
    int index = vfs_find_child(current_dir, name);
    int i;

    if (index < 0 || entries[index].is_dir)
        return 0;

    print(entries[index].data);

    i = 0;
    while (entries[index].data[i])
        i++;

    if (i > 0 && entries[index].data[i - 1] != '\n')
        print("\n");

    return 1;
}

int vfs_touch(const char* name) {
    int index;

    if (!name[0] ||
        vfs_compare(name, ".") ||
        vfs_compare(name, ".."))
        return 0;

    if (vfs_find_child(current_dir, name) >= 0)
        return 1;

    index = vfs_find_free();

    if (index < 0)
        return 0;

    entries[index].used = 1;
    entries[index].is_dir = 0;
    entries[index].parent = current_dir;

    vfs_copy(entries[index].name, name, VFS_NAME_SIZE);
    entries[index].data[0] = '\0';

    if (!vfs_save()) {
        entries[index].used = 0;
        return 0;
    }

    return 1;
}

int vfs_write(const char* name, const char* text) {
    int index = vfs_find_child(current_dir, name);
    char old_data[VFS_DATA_SIZE];
    int i;

    if (index < 0 || entries[index].is_dir)
        return 0;

    for (i = 0; i < VFS_DATA_SIZE; i++)
        old_data[i] = entries[index].data[i];

    vfs_copy(entries[index].data, text, VFS_DATA_SIZE);

    if (!vfs_save()) {
        for (i = 0; i < VFS_DATA_SIZE; i++)
            entries[index].data[i] = old_data[i];

        return 0;
    }

    return 1;
}

int vfs_remove(const char* name) {
    int index = vfs_find_child(current_dir, name);
    int old_used;

    if (index < 0 || entries[index].is_dir)
        return 0;

    old_used = entries[index].used;
    entries[index].used = 0;

    if (!vfs_save()) {
        entries[index].used = old_used;
        return 0;
    }

    return 1;
}

int vfs_mkdir(const char* name) {
    int index;

    if (!name[0] ||
        vfs_compare(name, ".") ||
        vfs_compare(name, ".."))
        return 0;

    if (vfs_find_child(current_dir, name) >= 0)
        return 1;

    index = vfs_find_free();

    if (index < 0)
        return 0;

    entries[index].used = 1;
    entries[index].is_dir = 1;
    entries[index].parent = current_dir;

    vfs_copy(entries[index].name, name, VFS_NAME_SIZE);
    entries[index].data[0] = '\0';

    if (!vfs_save()) {
        entries[index].used = 0;
        return 0;
    }

    return 1;
}

int vfs_cd(const char* name) {
    int index;

    if (vfs_compare(name, "/")) {
        current_dir = 0;
        return 1;
    }

    if (vfs_compare(name, "."))
        return 1;

    if (vfs_compare(name, "..")) {
        if (current_dir != 0)
            current_dir = entries[current_dir].parent;

        return 1;
    }

    index = vfs_find_child(current_dir, name);

    if (index < 0 || !entries[index].is_dir)
        return 0;

    current_dir = index;
    return 1;
}

void vfs_pwd() {
    int stack[VFS_MAX_ENTRIES];
    int count = 0;
    int index = current_dir;
    int i;

    if (current_dir == 0) {
        print("/\n");
        return;
    }

    while (index != 0 && count < VFS_MAX_ENTRIES) {
        stack[count++] = index;
        index = entries[index].parent;
    }

    print("/");

    for (i = count - 1; i >= 0; i--) {
        print(entries[stack[i]].name);

        if (i > 0)
            print("/");
    }

    print("\n");
}
int vfs_read_file(const char* name, char* buffer, int max_size) {
    int index;
    int i;

    if (max_size <= 0)
        return 0;

    index = vfs_find_child(current_dir, name);

    if (index < 0 || entries[index].is_dir)
        return 0;

    i = 0;

    while (entries[index].data[i] && i < max_size - 1) {
        buffer[i] = entries[index].data[i];
        i++;
    }

    buffer[i] = '\0';

    return i;
}
void vfs_list_programs() {
    int i;
    int j;
    int length;
    int found = 0;
    char name[VFS_NAME_SIZE];

    print("PROGRAMS\n");

    for (i = 0; i < VFS_MAX_ENTRIES; i++) {
        if (!entries[i].used || entries[i].is_dir)
            continue;

        length = 0;

        while (entries[i].name[length] != '\0')
            length++;

        if (length < 4)
            continue;

        if (entries[i].name[length - 4] != '.' ||
            entries[i].name[length - 3] != 'b' ||
            entries[i].name[length - 2] != 'i' ||
            entries[i].name[length - 1] != 'n')
            continue;

        for (j = 0; j < length - 4; j++)
            name[j] = entries[i].name[j];

        name[length - 4] = '\0';

        print(name);
        print("\n");
        found = 1;
    }

    if (!found)
        print("No programs found.\n");
}