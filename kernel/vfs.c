/*
 * ValaOS
 * Copyright (c) 2026 Nehal Vala
 * All rights reserved.
 */

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define VFS_MAX_ENTRIES 32
#define VFS_NAME_SIZE 32
#define VFS_DATA_SIZE 256

#define VFS_HEADER_SECTOR 1
#define VFS_LEGACY_DATA_START_SECTOR 2
#define VFS_LEGACY_DATA_SECTORS 10
#define VFS_DATA_START_SECTOR 21
#define VFS_DATA_SECTORS 19
#define VFS_ENTRY_SIZE 300
#define VFS_STORAGE_BYTES (VFS_MAX_ENTRIES * VFS_ENTRY_SIZE)

#define VFS_MAGIC 0x56414C41
#define VFS_VERSION 2

struct vfs_entry
{
    int used;
    int is_dir;
    int parent;
    char name[VFS_NAME_SIZE];
    char data[VFS_DATA_SIZE];
};

static struct vfs_entry entries[VFS_MAX_ENTRIES];
static int current_dir = 0;
static int vfs_mounted = 0;
static int vfs_unformatted = 0;
static uint8_t storage[VFS_DATA_SECTORS * 512];
static uint8_t legacy_storage[VFS_LEGACY_DATA_SECTORS * 512];

extern void print(const char *str);
extern void print_number(unsigned int number);
extern int vfs_current_dir_index(void);
extern int disk_read_sector(uint32_t lba, uint16_t *buffer);
extern int disk_write_sector(uint32_t lba, const uint16_t *buffer);
extern int disk_flush(void);
extern int disk_is_ready(void);
extern uint32_t disk_sector_count(void);
static int vfs_compare(const char *a, const char *b);

static int vfs_valid_name(const char *name)
{
    int i;

    if (!name || !name[0])
        return 0;
    if (vfs_compare(name, ".") || vfs_compare(name, ".."))
        return 0;

    for (i = 0; i < VFS_NAME_SIZE; i++)
    {
        unsigned char ch = (unsigned char)name[i];
        if (!ch)
            return i > 0;
        if (ch < 0x20 || ch == '/' || ch == '\\')
            return 0;
    }
    return 0;
}

static int vfs_validate_entries(void)
{
    int i;
    int j;

    if (entries[0].used != 1 || !entries[0].is_dir ||
        entries[0].parent != -1 || !vfs_compare(entries[0].name, "/"))
        return 0;

    for (i = 0; i < VFS_MAX_ENTRIES; i++)
    {
        int name_terminated = 0;
        int data_terminated = 0;
        int parent;

        if (entries[i].used != 0 && entries[i].used != 1)
            return 0;
        if (!entries[i].used)
            continue;
        if (entries[i].is_dir != 0 && entries[i].is_dir != 1)
            return 0;
        for (j = 0; j < VFS_NAME_SIZE; j++)
            if (!entries[i].name[j]) { name_terminated = 1; break; }
        for (j = 0; j < VFS_DATA_SIZE; j++)
            if (!entries[i].data[j]) { data_terminated = 1; break; }
        if (!name_terminated || !data_terminated)
            return 0;
        if (i == 0)
            continue;
        if (!vfs_valid_name(entries[i].name))
            return 0;
        parent = entries[i].parent;
        if (parent < 0 || parent >= VFS_MAX_ENTRIES ||
            !entries[parent].used || !entries[parent].is_dir)
            return 0;
        for (j = 0; j < i; j++)
            if (entries[j].used && entries[j].parent == parent &&
                vfs_compare(entries[j].name, entries[i].name))
                return 0;

        /* Parent chains must reach root and cannot contain cycles. */
        {
            int depth = 0;
            int ancestor = parent;
            while (ancestor != 0 && depth++ < VFS_MAX_ENTRIES)
            {
                if (ancestor < 0 || ancestor >= VFS_MAX_ENTRIES ||
                    !entries[ancestor].used || !entries[ancestor].is_dir)
                    return 0;
                ancestor = entries[ancestor].parent;
            }
            if (ancestor != 0)
                return 0;
        }
    }
    return 1;
}

static int vfs_require_mounted(void)
{
    if (vfs_mounted)
        return 1;
    print("[ VFS ] Filesystem is not mounted; use 'format' on a blank data image\n");
    return 0;
}

static int vfs_compare(const char *a, const char *b)
{
    while (*a && *b)
    {
        if (*a != *b)
            return 0;

        a++;
        b++;
    }

    return *a == *b;
}

static void vfs_copy(char *dest, const char *src, int limit)
{
    int i = 0;

    while (src[i] && i < limit - 1)
    {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
}

static int vfs_find_child(int parent, const char *name)
{
    int i;

    for (i = 0; i < VFS_MAX_ENTRIES; i++)
    {
        if (entries[i].used &&
            entries[i].parent == parent &&
            vfs_compare(entries[i].name, name))
            return i;
    }

    return -1;
}

static int vfs_find_free()
{
    int i;

    for (i = 0; i < VFS_MAX_ENTRIES; i++)
    {
        if (!entries[i].used)
            return i;
    }

    return -1;
}

static void vfs_clear_storage()
{
    int i;

    for (i = 0; i < VFS_DATA_SECTORS * 512; i++)
        storage[i] = 0;
}

static void vfs_put_u32(uint8_t *buffer, int *offset, uint32_t value)
{
    buffer[*offset + 0] = (uint8_t)(value & 0xFF);
    buffer[*offset + 1] = (uint8_t)((value >> 8) & 0xFF);
    buffer[*offset + 2] = (uint8_t)((value >> 16) & 0xFF);
    buffer[*offset + 3] = (uint8_t)((value >> 24) & 0xFF);

    *offset += 4;
}

static uint32_t vfs_get_u32(const uint8_t *buffer, int *offset)
{
    uint32_t value;

    value = (uint32_t)buffer[*offset + 0];
    value |= (uint32_t)buffer[*offset + 1] << 8;
    value |= (uint32_t)buffer[*offset + 2] << 16;
    value |= (uint32_t)buffer[*offset + 3] << 24;

    *offset += 4;

    return value;
}

static void vfs_serialize()
{
    int i;
    int j;
    int offset;

    vfs_clear_storage();
    offset = 0;

    for (i = 0; i < VFS_MAX_ENTRIES; i++)
    {
        vfs_put_u32(
            storage,
            &offset,
            (uint32_t)entries[i].used);

        vfs_put_u32(
            storage,
            &offset,
            (uint32_t)entries[i].is_dir);

        vfs_put_u32(
            storage,
            &offset,
            (uint32_t)entries[i].parent);

        for (j = 0; j < VFS_NAME_SIZE; j++)
            storage[offset++] = (uint8_t)entries[i].name[j];

        for (j = 0; j < VFS_DATA_SIZE; j++)
            storage[offset++] = (uint8_t)entries[i].data[j];
    }
}

static void vfs_deserialize()
{
    int i;
    int j;
    int offset;

    offset = 0;

    for (i = 0; i < VFS_MAX_ENTRIES; i++)
    {
        entries[i].used = (int)vfs_get_u32(storage, &offset);
        entries[i].is_dir = (int)vfs_get_u32(storage, &offset);
        entries[i].parent = (int)vfs_get_u32(storage, &offset);

        for (j = 0; j < VFS_NAME_SIZE; j++)
            entries[i].name[j] = (char)storage[offset++];

        for (j = 0; j < VFS_DATA_SIZE; j++)
            entries[i].data[j] = (char)storage[offset++];

    }
}

static int vfs_read_sector(uint32_t lba, uint8_t *buffer)
{
    uint16_t words[256];
    int i;

    if (!((lba == VFS_HEADER_SECTOR) ||
          (lba >= VFS_LEGACY_DATA_START_SECTOR &&
           lba < VFS_DATA_START_SECTOR + VFS_DATA_SECTORS)) ||
        !disk_read_sector(lba, words))
        return 0;

    for (i = 0; i < 256; i++)
    {
        buffer[i * 2] = (uint8_t)(words[i] & 0xFF);
        buffer[i * 2 + 1] = (uint8_t)(words[i] >> 8);
    }

    return 1;
}

static int vfs_write_sector(uint32_t lba, const uint8_t *buffer)
{
    uint16_t words[256];
    int i;

    if (!((lba == VFS_HEADER_SECTOR) ||
          (lba >= VFS_DATA_START_SECTOR &&
           lba < VFS_DATA_START_SECTOR + VFS_DATA_SECTORS)))
        return 0;

    for (i = 0; i < 256; i++)
        words[i] = (uint16_t)buffer[i * 2] |
                   ((uint16_t)buffer[i * 2 + 1] << 8);

    return disk_write_sector(lba, words);
}

static int vfs_save()
{
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

    for (sector = 0; sector < VFS_DATA_SECTORS; sector++)
    {
        if (!vfs_write_sector(
                VFS_DATA_START_SECTOR + sector,
                &storage[sector * 512]))
        {
            print("[ ERROR ] VFS data write failed at LBA ");
            print_number(VFS_DATA_START_SECTOR + (unsigned int)sector);
            print("\n");
            return 0;
        }
    }

    if (!disk_flush())
    {
        print("[ ERROR ] VFS data flush failed\n");
        return 0;
    }

    if (!vfs_write_sector(VFS_HEADER_SECTOR, header))
    {
        print("[ ERROR ] VFS header write failed at LBA 1\n");
        return 0;
    }

    if (!disk_flush())
    {
        print("[ ERROR ] VFS header flush failed\n");
        return 0;
    }

    return 1;
}

static void vfs_deserialize_legacy()
{
    int i;
    int j;
    int offset = 0;

    for (i = 0; i < VFS_MAX_ENTRIES; i++)
    {
        entries[i].used = 0;
        entries[i].is_dir = 0;
        entries[i].parent = -1;
        entries[i].name[0] = '\0';
        entries[i].data[0] = '\0';
    }

    for (i = 0; i < 16; i++)
    {
        entries[i].used = (int)vfs_get_u32(legacy_storage, &offset);
        entries[i].is_dir = (int)vfs_get_u32(legacy_storage, &offset);
        entries[i].parent = (int)vfs_get_u32(legacy_storage, &offset);

        for (j = 0; j < VFS_NAME_SIZE; j++)
            entries[i].name[j] = (char)legacy_storage[offset++];

        for (j = 0; j < VFS_DATA_SIZE; j++)
            entries[i].data[j] = (char)legacy_storage[offset++];

        entries[i].name[VFS_NAME_SIZE - 1] = '\0';
        entries[i].data[VFS_DATA_SIZE - 1] = '\0';
    }
}

static int vfs_load()
{
    uint8_t header[512];
    int offset = 0;
    uint32_t magic;
    uint32_t version;
    uint32_t entries_count;
    uint32_t entry_size;
    uint32_t sector_count;
    int sector;
    int i;
    int blank = 1;
    uint8_t blank_check[512];

    if (!vfs_read_sector(VFS_HEADER_SECTOR, header))
        return 0;

    for (i = 0; i < 512; i++)
    {
        if (header[i] != 0)
        {
            blank = 0;
            break;
        }
    }

    if (blank)
    {
        /* Treat as unformatted only when every VFS-reserved sector is blank. */
        for (sector = VFS_LEGACY_DATA_START_SECTOR;
             sector < VFS_DATA_START_SECTOR + VFS_DATA_SECTORS;
             sector++)
        {
            if (!vfs_read_sector((uint32_t)sector, blank_check))
                return 0;
            for (i = 0; i < 512; i++)
                if (blank_check[i] != 0)
                    return 0;
        }
        return 3;
    }

    magic = vfs_get_u32(header, &offset);
    version = vfs_get_u32(header, &offset);
    entries_count = vfs_get_u32(header, &offset);
    entry_size = vfs_get_u32(header, &offset);
    sector_count = vfs_get_u32(header, &offset);

    if (magic != VFS_MAGIC || entry_size != VFS_ENTRY_SIZE)
        return 0;

    if (version == 1 &&
        entries_count == 16 &&
        sector_count == VFS_LEGACY_DATA_SECTORS)
    {
        for (sector = 0; sector < VFS_LEGACY_DATA_SECTORS; sector++)
        {
            if (!vfs_read_sector(
                    VFS_LEGACY_DATA_START_SECTOR + sector,
                    &legacy_storage[sector * 512]))
                return 0;
        }

        vfs_deserialize_legacy();

        if (!vfs_validate_entries())
            return 0;

        current_dir = 0;
        return 2;
    }

    if (version != VFS_VERSION ||
        entries_count != VFS_MAX_ENTRIES ||
        sector_count != VFS_DATA_SECTORS)
        return 0;

    for (sector = 0; sector < VFS_DATA_SECTORS; sector++)
    {
        if (!vfs_read_sector(
                VFS_DATA_START_SECTOR + sector,
                &storage[sector * 512]))
            return 0;
    }

    vfs_deserialize();

    if (!vfs_validate_entries())
        return 0;

    current_dir = 0;
    return 1;
}

static void vfs_create_default()
{
    int i;

    for (i = 0; i < VFS_MAX_ENTRIES; i++)
    {
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
        VFS_DATA_SIZE);

    entries[2].used = 1;
    entries[2].is_dir = 0;
    entries[2].parent = 0;
    vfs_copy(entries[2].name, "hello.txt", VFS_NAME_SIZE);
    vfs_copy(
        entries[2].data,
        "Hello from ValaOS.\n",
        VFS_DATA_SIZE);

    entries[3].used = 1;
    entries[3].is_dir = 0;
    entries[3].parent = 0;
    vfs_copy(entries[3].name, "system.txt", VFS_NAME_SIZE);
    vfs_copy(
        entries[3].data,
        "ValaOS 2.0\nArchitecture: x86\nFilesystem: VFS\n",
        VFS_DATA_SIZE);

    entries[4].used = 1;
    entries[4].is_dir = 0;
    entries[4].parent = 0;
    vfs_copy(entries[4].name, "hello.bin", VFS_NAME_SIZE);
    vfs_copy(
        entries[4].data,
        "B801000000BB55000000CD80B80200000031DBCD80EBFE",
        VFS_DATA_SIZE);

    current_dir = 0;
}

void vfs_init()
{
    int i;
    int used_count = 0;
    int status;

    vfs_mounted = 0;
    vfs_unformatted = 0;
    if (!disk_is_ready())
    {
        print("[ DISK ] Dedicated ValaOS data disk unavailable\n");
        return;
    }

    status = vfs_load();

    if (status == 1)
    {
        vfs_mounted = 1;
        for (i = 0; i < VFS_MAX_ENTRIES; i++)
        {
            if (entries[i].used == 1)
                used_count++;
        }

        print("[ DEBUG ] VFS version 2 loaded\n");
        print("[ DEBUG ] Used entries: ");
        print_number((unsigned int)used_count);
        print("\n");
        return;
    }

    if (status == 2)
    {
        print("[ DEBUG ] VFS version 1 detected\n");
        print("[ DEBUG ] Migrating to version 2\n");

        if (vfs_save())
        {
            vfs_mounted = 1;
            print("[ DEBUG ] VFS migration successful\n");
            return;
        }

        print("[ ERROR ] VFS migration failed; legacy data preserved\n");
        return;
    }

    if (status == 3)
    {
        vfs_unformatted = 1;
        print("[ VFS ] Blank data image; run 'format' to initialize it\n");
        return;
    }

    print("[ ERROR ] Invalid VFS or disk read failure\n");
    print("[ ERROR ] Existing disk data was not reset\n");
}

int vfs_format(void)
{
    int status;

    if (!disk_is_ready())
    {
        print("[ VFS ] Dedicated data image is not available\n");
        return 0;
    }
    if (vfs_mounted)
    {
        print("[ VFS ] Refusing to format a mounted filesystem\n");
        return 0;
    }
    status = vfs_unformatted ? 3 : vfs_load();
    if (status != 3)
    {
        print("[ VFS ] Format allowed only on a blank, unformatted image\n");
        return 0;
    }

    /* Consume the blank-image authorization before the first disk write. */
    vfs_unformatted = 0;
    vfs_create_default();
    if (!vfs_save())
    {
        print("[ VFS ] Format failed while writing metadata\n");
        return 0;
    }
    if (!vfs_validate_entries())
    {
        print("[ VFS ] Formatted metadata failed validation\n");
        return 0;
    }
    vfs_mounted = 1;
    print("[ VFS ] Formatted ValaOS VFS v2 on dedicated data image\n");
    return 1;
}

void vfs_info(void)
{
    int i;
    int used = 0;

    print("ValaOS VFS v2\n");
    print("Block size: 512 bytes\n");
    print("Data image: ");
    print_number(disk_sector_count());
    print(" sectors (");
    print_number((disk_sector_count() * 512U) / 1024U);
    print(" KiB)\n");
    if (!vfs_mounted)
    {
        print("Status: not mounted\n");
        return;
    }
    for (i = 0; i < VFS_MAX_ENTRIES; i++)
        if (entries[i].used)
            used++;
    print("Status: mounted\nDirectory entries: ");
    print_number((unsigned int)used);
    print(" / ");
    print_number(VFS_MAX_ENTRIES);
    print(" (including root)\nMax file content: ");
    print_number(VFS_DATA_SIZE - 1);
    print(" bytes\n");
}

void vfs_list()
{
    int i;

    if (!vfs_require_mounted())
        return;
    print("NAME\n");

    for (i = 0; i < VFS_MAX_ENTRIES; i++)
    {
        if (entries[i].used && entries[i].parent == current_dir)
        {
            print(entries[i].name);

            if (entries[i].is_dir)
                print("/");

            print("\n");
        }
    }
}

int vfs_list_dir(const char *name)
{
    int index;
    int i;

    if (!vfs_require_mounted())
        return 0;
    index = vfs_find_child(current_dir, name);

    if (index < 0 || !entries[index].is_dir)
        return 0;

    print("NAME\n");

    for (i = 0; i < VFS_MAX_ENTRIES; i++)
    {
        if (entries[i].used && entries[i].parent == index)
        {
            print(entries[i].name);

            if (entries[i].is_dir)
                print("/");

            print("\n");
        }
    }

    return 1;
}

int vfs_complete_name(const char *prefix, int parent, int index,
                      char *name, int max_size, int *is_dir)
{
    int i;
    int j = 0;
    int prefix_len = 0;

    if (!vfs_mounted || !prefix || !name || !is_dir || max_size <= 0)
        return 0;
    while (prefix[prefix_len] && prefix_len < VFS_NAME_SIZE)
        prefix_len++;
    if (prefix_len == VFS_NAME_SIZE)
        return 0;

    for (i = 0; i < VFS_MAX_ENTRIES; i++)
    {
        if (!entries[i].used || entries[i].parent != parent)
            continue;

        for (j = 0; j < prefix_len; j++)
        {
            if (entries[i].name[j] != prefix[j])
                break;
        }

        if (j != prefix_len)
            continue;

        if (index > 0)
        {
            index--;
            continue;
        }

        j = 0;

        while (entries[i].name[j] && j < max_size - 2)
        {
            name[j] = entries[i].name[j];
            j++;
        }

        name[j] = '\0';
        *is_dir = entries[i].is_dir;

        return 1;
    }

    return 0;
}

int vfs_cat(const char *name)
{
    int index;
    int i;

    if (!vfs_require_mounted())
        return 0;
    index = vfs_find_child(current_dir, name);

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

int vfs_touch(const char *name)
{
    int index;

    if (!vfs_require_mounted() || !vfs_valid_name(name))
        return 0;

    index = vfs_find_child(current_dir, name);
    if (index >= 0)
        return !entries[index].is_dir;

    index = vfs_find_free();

    if (index < 0)
    {
        return 0;
    }

    entries[index].used = 1;
    entries[index].is_dir = 0;
    entries[index].parent = current_dir;

    vfs_copy(entries[index].name, name, VFS_NAME_SIZE);
    entries[index].data[0] = '\0';

    if (!vfs_save())
    {
        entries[index].used = 0;
        return 0;
    }

    return 1;
}

int vfs_write(const char *name, const char *text)
{
    int index;
    char old_data[VFS_DATA_SIZE];
    int i;
    int length = 0;
    int created = 0;

    if (!vfs_require_mounted() || !vfs_valid_name(name) || !text)
        return 0;
    while (length < VFS_DATA_SIZE && text[length])
        length++;
    if (length >= VFS_DATA_SIZE)
        return 0;
    index = vfs_find_child(current_dir, name);

    if (index >= 0 && entries[index].is_dir)
        return 0;
    if (index < 0)
    {
        index = vfs_find_free();
        if (index < 0)
            return 0;
        created = 1;
        entries[index].used = 1;
        entries[index].is_dir = 0;
        entries[index].parent = current_dir;
        vfs_copy(entries[index].name, name, VFS_NAME_SIZE);
        entries[index].data[0] = '\0';
    }

    for (i = 0; i < VFS_DATA_SIZE; i++)
        old_data[i] = entries[index].data[i];

    vfs_copy(entries[index].data, text, VFS_DATA_SIZE);

    if (!vfs_save())
    {
        if (created)
            entries[index].used = 0;
        else
            for (i = 0; i < VFS_DATA_SIZE; i++)
                entries[index].data[i] = old_data[i];

        return 0;
    }

    return 1;
}

int vfs_remove(const char *name)
{
    int index;
    int old_used;

    if (!vfs_require_mounted())
        return 0;
    index = vfs_find_child(current_dir, name);

    if (index < 0 || entries[index].is_dir)
        return 0;

    old_used = entries[index].used;
    entries[index].used = 0;

    if (!vfs_save())
    {
        entries[index].used = old_used;
        return 0;
    }

    return 1;
}

int vfs_rmdir(const char *name)
{
    int index;
    int i;
    int old_used;

    if (!vfs_require_mounted() || !name || !name[0] ||
        vfs_compare(name, ".") ||
        vfs_compare(name, ".."))
        return 0;

    index = vfs_find_child(current_dir, name);

    if (index <= 0 || !entries[index].is_dir)
        return 0;

    for (i = 0; i < VFS_MAX_ENTRIES; i++)
    {
        if (entries[i].used && entries[i].parent == index)
            return 0;
    }

    old_used = entries[index].used;
    entries[index].used = 0;

    if (!vfs_save())
    {
        entries[index].used = old_used;
        return 0;
    }

    return 1;
}

int vfs_mkdir(const char *name)
{
    int index;

    if (!vfs_require_mounted() || !vfs_valid_name(name))
        return 0;

    index = vfs_find_child(current_dir, name);
    if (index >= 0)
        return entries[index].is_dir;

    index = vfs_find_free();

    if (index < 0)
        return 0;

    entries[index].used = 1;
    entries[index].is_dir = 1;
    entries[index].parent = current_dir;

    vfs_copy(entries[index].name, name, VFS_NAME_SIZE);
    entries[index].data[0] = '\0';

    if (!vfs_save())
    {
        entries[index].used = 0;
        return 0;
    }

    return 1;
}

int vfs_cd(const char *name)
{
    int index;

    if (!vfs_require_mounted() || !name)
        return 0;

    if (vfs_compare(name, "/"))
    {
        current_dir = 0;
        return 1;
    }

    if (vfs_compare(name, "."))
        return 1;

    if (vfs_compare(name, ".."))
    {
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

void vfs_pwd()
{
    int stack[VFS_MAX_ENTRIES];
    int count = 0;
    int index = current_dir;
    int i;

    if (!vfs_require_mounted())
        return;
    if (current_dir == 0)
    {
        print("/\n");
        return;
    }

    while (index != 0 && count < VFS_MAX_ENTRIES)
    {
        stack[count++] = index;
        index = entries[index].parent;
    }

    print("/");

    for (i = count - 1; i >= 0; i--)
    {
        print(entries[stack[i]].name);

        if (i > 0)
            print("/");
    }

    print("\n");
}

int vfs_current_dir_index(void)
{
    return current_dir;
}

int vfs_read_file(const char *name, char *buffer, int max_size)
{
    int index;
    int i;

    if (!vfs_require_mounted() || !name || !buffer || max_size <= 0)
        return 0;

    index = vfs_find_child(current_dir, name);

    if (index < 0 || entries[index].is_dir)
        return 0;

    i = 0;

    while (entries[index].data[i] && i < max_size - 1)
    {
        buffer[i] = entries[index].data[i];
        i++;
    }

    buffer[i] = '\0';

    return i;
}
void vfs_list_programs()
{
    int i;
    int j;
    int length;
    int found = 0;
    char name[VFS_NAME_SIZE];

    if (!vfs_require_mounted())
        return;
    print("PROGRAMS\n");

    for (i = 0; i < VFS_MAX_ENTRIES; i++)
    {
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
