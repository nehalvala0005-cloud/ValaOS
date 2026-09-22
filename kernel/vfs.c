#define VFS_MAX_FILES 8
#define VFS_NAME_SIZE 32
#define VFS_DATA_SIZE 256

struct vfs_file {
    int used;
    char name[VFS_NAME_SIZE];
    char data[VFS_DATA_SIZE];
};

static struct vfs_file files[VFS_MAX_FILES];

extern void print(const char* str);

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

static int vfs_find(const char* name) {
    int i;

    for (i = 0; i < VFS_MAX_FILES; i++) {
        if (files[i].used && vfs_compare(files[i].name, name))
            return i;
    }

    return -1;
}

void vfs_init() {
    int i;

    for (i = 0; i < VFS_MAX_FILES; i++)
        files[i].used = 0;

    files[0].used = 1;
    vfs_copy(files[0].name, "readme.txt", VFS_NAME_SIZE);
    vfs_copy(files[0].data,
             "Welcome to ValaOS!\nThis is the ValaOS virtual file system.\n",
             VFS_DATA_SIZE);

    files[1].used = 1;
    vfs_copy(files[1].name, "hello.txt", VFS_NAME_SIZE);
    vfs_copy(files[1].data,
             "Hello from ValaOS.\n",
             VFS_DATA_SIZE);

    files[2].used = 1;
    vfs_copy(files[2].name, "system.txt", VFS_NAME_SIZE);
    vfs_copy(files[2].data,
             "ValaOS 2.0\nArchitecture: x86\nFilesystem: VFS\n",
             VFS_DATA_SIZE);
}

void vfs_list() {
    int i;

    print("NAME\n");

    for (i = 0; i < VFS_MAX_FILES; i++) {
        if (files[i].used) {
            print(files[i].name);
            print("\n");
        }
    }
}

int vfs_cat(const char* name) {
    int index = vfs_find(name);

    if (index < 0)
        return 0;

    print(files[index].data);
    return 1;
}

int vfs_touch(const char* name) {
    int i;
    int index;

    if (!name[0])
        return 0;

    index = vfs_find(name);

    if (index >= 0)
        return 1;

    for (i = 0; i < VFS_MAX_FILES; i++) {
        if (!files[i].used) {
            files[i].used = 1;
            vfs_copy(files[i].name, name, VFS_NAME_SIZE);
            files[i].data[0] = '\0';
            return 1;
        }
    }

    return 0;
}

int vfs_write(const char* name, const char* text) {
    int index = vfs_find(name);

    if (index < 0)
        return 0;

    vfs_copy(files[index].data, text, VFS_DATA_SIZE);
    return 1;
}

int vfs_remove(const char* name) {
    int index = vfs_find(name);

    if (index < 0)
        return 0;

    files[index].used = 0;
    return 1;
}
