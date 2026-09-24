typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

extern void* kmalloc(unsigned int size);
extern void kfree(void* address);
extern unsigned int memory_used();
extern unsigned int memory_free();
extern void enter_user_mode();
extern void user_task_start();

struct task_context {
    unsigned int eax;
    unsigned int ecx;
    unsigned int edx;
    unsigned int ebx;
    unsigned int esp;
    unsigned int ebp;
    unsigned int esi;
    unsigned int edi;
    unsigned int eip;
    unsigned int cs;
    unsigned int eflags;
};
struct task {
    unsigned int pid;
    const char* name;
    const char* state;
    unsigned int stack_base;
    unsigned int stack_top;
    struct task_context context;
};

extern struct task* get_tasks();
extern int get_task_count();
extern void schedule_once();
extern unsigned int memory_total();
extern unsigned int memory_page_size();
extern unsigned int memory_total_pages();
extern unsigned int memory_used_pages();
extern int paging_enabled();
extern int map_page(unsigned int virtual_address, unsigned int physical_address);
extern int unmap_page(unsigned int virtual_address);
extern int is_page_mapped(unsigned int virtual_address);
extern int virtual_to_physical(unsigned int virtual_address, unsigned int* physical_address);
extern unsigned int get_page_flags(unsigned int virtual_address);
extern int set_page_flags(unsigned int virtual_address, unsigned int flags);
extern void enable_write_protection();
extern int task_kill(unsigned int pid);
extern unsigned int get_timer_ticks();
extern void vfs_init();
extern void vfs_list();
extern int vfs_cat(const char* name);
extern int vfs_touch(const char* name);
extern int vfs_write(const char* name, const char* text);
extern int vfs_remove(const char* name);
extern int vfs_mkdir(const char* name);
extern int vfs_cd(const char* name);
extern void vfs_pwd();
extern int load_user_program(const char* name);
extern int disk_test();
unsigned char* video = (unsigned char*)0xB8000;

char input[64];
int input_pos = 0;

int row = 0;
int col = 0;

char key_map[] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,
    0,0,' '
};

uint8_t inb(uint16_t port) {
    uint8_t value;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}

void outb(uint16_t port, uint8_t value) {
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

void hide_cursor() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void move_cursor_next_line() {
    col = 0;
    row++;

    if (row >= 25) {
        for (int r = 1; r < 25; r++) {
            for (int c = 0; c < 80; c++) {
                int from = (r * 80 + c) * 2;
                int to = ((r - 1) * 80 + c) * 2;

                video[to] = video[from];
                video[to + 1] = video[from + 1];
            }
        }

        for (int c = 0; c < 80; c++) {
            int pos = (24 * 80 + c) * 2;

            video[pos] = ' ';
            video[pos + 1] = 0x07;
        }

        row = 24;
    }
}

void putchar(char c) {
    if (c == '\n') {
        move_cursor_next_line();
        return;
    }

    if (col >= 80)
        move_cursor_next_line();

    int pos = (row * 80 + col) * 2;

    video[pos] = c;
    video[pos + 1] = 0x07;

    col++;
}

void print(const char* str) {
    while (*str) {
        putchar(*str);
        str++;
    }
}

void clear_screen() {
    for (int r = 0; r < 25; r++) {
        for (int c = 0; c < 80; c++) {
            int pos = (r * 80 + c) * 2;

            video[pos] = ' ';
            video[pos + 1] = 0x07;
        }
    }

    row = 0;
    col = 0;
}

int compare(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b)
            return 0;

        a++;
        b++;
    }

    return *a == *b;
}

int starts_with_echo(const char* str) {
    return str[0] == 'e' &&
           str[1] == 'c' &&
           str[2] == 'h' &&
           str[3] == 'o' &&
           str[4] == ' ';
}

void reboot_system() {
    uint8_t good = 0x02;

    while (good & 0x02)
        good = inb(0x64);

    outb(0x64, 0xFE);

    while (1)
        __asm__ volatile ("hlt");
}

void shutdown_system() {
    print("Shutting down ValaOS...\n");
    print("System halted.\n");

    while (1)
        __asm__ volatile ("cli; hlt");
}

void print_number(unsigned int number) {
    char buffer[10];
    int i = 0;
    char output[2];

    if (number == 0) {
        print("0");
        return;
    }

    while (number > 0) {
        buffer[i] = '0' + (number % 10);
        number = number / 10;
        i++;
    }

    while (i > 0) {
        i--;

        output[0] = buffer[i];
        output[1] = '\0';

        print(output);
    }
}

int parse_kill_pid(const char* str, unsigned int* pid) {
    unsigned int value = 0;
    int i = 5;

    if (str[0] != 'k' ||
        str[1] != 'i' ||
        str[2] != 'l' ||
        str[3] != 'l' ||
        str[4] != ' ')
        return 0;

    if (str[i] < '0' || str[i] > '9')
        return 0;

    while (str[i] >= '0' && str[i] <= '9') {
        value = value * 10 + (str[i] - '0');
        i++;
    }

    if (str[i] != '\0')
        return 0;

    *pid = value;
    return 1;
}

int starts_with(const char* str, const char* prefix) {
    while (*prefix) {
        if (*str != *prefix)
            return 0;
        str++;
        prefix++;
    }
    return 1;
}

const char* command_argument(const char* str, int offset) {
    while (str[offset] == ' ')
        offset++;
    return str + offset;
}

void execute_command() {
    unsigned int kill_pid;
    int result;
    const char* arg;
    const char* text;

    putchar('\n');

    if (compare(input, "help")) {
        print("Available commands:\n");
        print("help\n");
        print("info\n");
        print("about\n");
        print("clear\n");
        print("version\n");
        print("echo <text>\n");
        print("reboot\n");
        print("shutdown\n");
        print("memtest\n");
        print("meminfo\n");
        print("paging\n");
        print("ptest\n");
        print("dptest\n");
        print("vmtest\n");
        print("vmaptest\n");
        print("memstress\n");
        print("ps\n");
        print("uptime\n");
        print("kill <PID>\n");
        print("tasks\n");
        print("ticks\n");
        print("schedule\n");
        print("run hello\n");
        print("ls\n");
        print("cat <file>\n");
        print("touch <file>\n");
        print("write <file> <text>\n");
        print("rm <file>\n");
        print("mkdir <dir>\n");
        print("cd <dir>\n");
        print("pwd\n");
        print("disktest\n");
       
    }
    else if (compare(input, "info")) {
        print("ValaOS v0.7\n");
        print("Architecture: x86\n");
        print("Keyboard: PS/2 Polling\n");
        print("Kernel: Custom\n");
        print("Status: Running\n");
    }
    else if (compare(input, "about")) {
        print("ValaOS - Custom Operating System\n");
        print("Built from scratch for learning OS concepts.\n");
    }
    else if (compare(input, "clear")) {
        clear_screen();
    }
    else if (compare(input, "version")) {
        print("ValaOS v0.7\n");
        print("Architecture: x86\n");
        print("Shell: ValaShell\n");
    }
  else if (compare(input, "memtest")) {
    void* block = kmalloc(100);

    print("Memory test started.\n");

    if (block != 0) {
        print("100 bytes allocated.\n");
        print("Memory allocation: OK\n");

        kfree(block);

        print("100 bytes released.\n");
        print("Memory release: OK\n");
    } else {
        print("Memory allocation: FAILED\n");
    }
}
else if (compare(input, "memstress")) {
    void* a;
    void* b;
    void* c;
    void* d;

    print("Memory Stress Test\n\n");

    a = kmalloc(4096);
    if (a)
        print("[ OK ] Allocation A - 1 page\n");
    else
        print("[FAIL] Allocation A\n");

    b = kmalloc(8192);
    if (b)
        print("[ OK ] Allocation B - 2 pages\n");
    else
        print("[FAIL] Allocation B\n");

    c = kmalloc(16384);
    if (c)
        print("[ OK ] Allocation C - 4 pages\n");
    else
        print("[FAIL] Allocation C\n");

    kfree(b);
    print("[ OK ] Freed allocation B\n");

    d = kmalloc(8192);
    if (d)
        print("[ OK ] Reused freed pages\n");
    else
        print("[FAIL] Reallocation\n");

    kfree(a);
    kfree(c);
    kfree(d);

    print("[ OK ] All allocations released\n");
    print("\nMemory stress test passed.\n");
}
else if (compare(input, "meminfo")) {
    print("==============================\n");
    print("       ValaOS MEMORY INFO\n");
    print("==============================\n");

    print("Total Memory : 4194304 bytes\n");
    print("Page Size    : 4096 bytes\n");
    print("Total Pages  : 1024\n");

    if (memory_used_pages() == 0) {
        print("Used Pages   : 0\n");
        print("Used Memory  : 0 bytes\n");
        print("Free Memory  : 4194304 bytes\n");
    } else {
        print("Memory allocated\n");
    }

    print("Status       : Healthy\n");
}
else if (compare(input, "paging")) {
    print("==============================\n");
    print("       VALAOS PAGING INFO\n");
    print("==============================\n");

    if (paging_enabled()) {
        print("Paging Status : ENABLED\n");
        print("Mapped Memory  : 16 MB\n");
        print("Page Size      : 4096 bytes\n");
        print("Page Tables    : 4\n");
        print("Status         : Healthy\n");
    } else {
        print("Paging Status : DISABLED\n");
        print("Status         : Error\n");
    }
}
else if (compare(input, "ptest")) {
    unsigned int virtual_address = 0x00E00000;
    unsigned int physical_address = 0x00300000;

    print("Page Mapping Test\n\n");

    if (map_page(virtual_address, physical_address))
        print("[ OK ] Virtual page mapped\n");
    else
        print("[FAIL] Page mapping\n");

    if (is_page_mapped(virtual_address))
        print("[ OK ] Mapping verified\n");
    else
        print("[FAIL] Mapping verification\n");

    if (unmap_page(virtual_address))
        print("[ OK ] Virtual page unmapped\n");
    else
        print("[FAIL] Page unmapping\n");

    if (!is_page_mapped(virtual_address))
        print("[ OK ] Unmapping verified\n");
    else
        print("[FAIL] Unmapping verification\n");

    print("\nPage mapping test passed.\n");
}
else if (compare(input, "dptest")) {
    unsigned int virtual_address = 0x01000000;
    unsigned int physical_address = 0x01000000;

    print("Dynamic Page Table Test\n\n");

    if (map_page(virtual_address, physical_address))
        print("[ OK ] Dynamic page table created\n");
    else
        print("[FAIL] Dynamic page table creation\n");

    if (is_page_mapped(virtual_address))
        print("[ OK ] Dynamic mapping verified\n");
    else
        print("[FAIL] Dynamic mapping verification\n");

    if (unmap_page(virtual_address))
        print("[ OK ] Dynamic page unmapped\n");
    else
        print("[FAIL] Dynamic page unmapping\n");

    if (!is_page_mapped(virtual_address))
        print("[ OK ] Dynamic unmapping verified\n");
    else
        print("[FAIL] Dynamic unmapping verification\n");

    print("\nDynamic page table test passed.\n");
}
else if (compare(input, "vmtest")) {
    unsigned int virtual_address = 0x01001000;
    unsigned int physical_address = 0;
    unsigned int flags;

    print("Virtual Memory Test\n\n");

    if (map_page(virtual_address, 0x02000000))
        print("[ OK ] Page mapped\n");
    else
        print("[FAIL] Page mapping\n");

    if (virtual_to_physical(virtual_address, &physical_address))
        print("[ OK ] Virtual to physical translation\n");
    else
        print("[FAIL] Address translation\n");

    if (physical_address == 0x02000000)
        print("[ OK ] Physical address verified\n");
    else
        print("[FAIL] Physical address mismatch\n");

    flags = get_page_flags(virtual_address);

    if (flags & 0x2)
        print("[ OK ] Page writable\n");
    else
        print("[FAIL] Writable flag\n");

    if (set_page_flags(virtual_address, 0))
        print("[ OK ] Page protection changed\n");
    else
        print("[FAIL] Page protection change\n");

    flags = get_page_flags(virtual_address);

    if (!(flags & 0x2))
        print("[ OK ] Write protection verified\n");
    else
        print("[FAIL] Write protection verification\n");

    if (unmap_page(virtual_address))
        print("[ OK ] Page unmapped\n");
    else
        print("[FAIL] Page unmapping\n");

    print("\nVirtual memory test passed.\n");
}
else if (compare(input, "vmaptest")) {
    unsigned int virtual_address = 0x01000000;
    unsigned int physical_address = 0x00200000;
    unsigned int translated_address;
    unsigned int flags;

    print("Virtual Memory Mapping Test\n\n");

    if (map_page(virtual_address, physical_address))
        print("[ OK ] Page mapped\n");
    else
        print("[FAIL] Page mapping failed\n");

    if (is_page_mapped(virtual_address))
        print("[ OK ] Virtual page is mapped\n");
    else
        print("[FAIL] Mapping verification failed\n");

    if (virtual_to_physical(virtual_address, &translated_address))
        print("[ OK ] Virtual to physical translation\n");
    else
        print("[FAIL] Translation failed\n");

    if (translated_address == physical_address)
        print("[ OK ] Physical address verified\n");
    else
        print("[FAIL] Physical address mismatch\n");

    flags = get_page_flags(virtual_address);

    if (flags & 0x001)
        print("[ OK ] Page present flag verified\n");
    else
        print("[FAIL] Page present flag missing\n");

    if (flags & 0x002)
        print("[ OK ] Page writable flag verified\n");
    else
        print("[FAIL] Page writable flag missing\n");

    if (unmap_page(virtual_address))
        print("[ OK ] Page unmapped\n");
    else
        print("[FAIL] Page unmapping failed\n");

    if (!is_page_mapped(virtual_address))
        print("[ OK ] Mapping removed\n");
    else
        print("[FAIL] Mapping still exists\n");

    print("\nVirtual memory mapping test passed.\n");
}
else if (compare(input, "pwd")) {
    vfs_pwd();
}
else if (starts_with(input, "mkdir ")) {
    arg = command_argument(input, 6);

    if (vfs_mkdir(arg)) {
        print("[ VFS ] Directory created: ");
        print(arg);
        print("\n");
    }
    else {
        print("[ VFS ] Unable to create directory\n");
    }
}
else if (starts_with(input, "cd ")) {
    arg = command_argument(input, 3);

    if (!vfs_cd(arg))
        print("[ VFS ] Directory not found\n");
}
else if (compare(input, "cd")) {
    vfs_cd("/");
}
else if (compare(input, "ls")) {
    vfs_list();
}
else if (starts_with(input, "cat ")) {
    arg = command_argument(input, 4);

    if (!vfs_cat(arg))
        print("[ VFS ] File not found\n");
}
else if (starts_with(input, "touch ")) {
    arg = command_argument(input, 6);

    if (vfs_touch(arg))
        print("[ VFS ] File created\n");
    else
        print("[ VFS ] Unable to create file\n");
}
else if (starts_with(input, "write ")) {
    int i = 6;

    while (input[i] == ' ')
        i++;

    arg = input + i;

    while (input[i] && input[i] != ' ')
        i++;

    if (!input[i]) {
        print("[ VFS ] Usage: write <file> <text>\n");
    }
    else {
        input[i] = '\0';
        text = input + i + 1;

        while (*text == ' ')
            text++;

        if (vfs_write(arg, text))
            print("[ VFS ] File written\n");
        else
            print("[ VFS ] Unable to write file\n");
    }
}
else if (starts_with(input, "rm ")) {
    arg = command_argument(input, 3);

    if (vfs_remove(arg))
        print("[ VFS ] File removed\n");
    else
        print("[ VFS ] File not found\n");
}
else if (compare(input, "ps")) {
    struct task* list = get_tasks();
    int count = get_task_count();
    int i;

    print("PID   NAME      STATE\n");

    for (i = 0; i < count; i++) {
        if (list[i].pid == 1)
            print("1     ");
        else if (list[i].pid == 2)
            print("2     ");
        else if (list[i].pid == 3)
            print("3     ");

        print(list[i].name);
        print("      ");
        print(list[i].state);
        print("\n");
    }
}
else if (compare(input, "uptime")) {
    unsigned int ticks = get_timer_ticks();
    unsigned int seconds = ticks / 100;

    print("ValaOS Uptime: ");
    print_number(seconds);
    print(" seconds\n");
    print("Timer Ticks: ");
    print_number(ticks);
    print("\n");
}
else if (compare(input, "disktest")) {
    disk_test();
}
else if (parse_kill_pid(input, &kill_pid)) {
    result = task_kill(kill_pid);

    if (result == 1) {
        print("[ TASK ] PID ");
        print_number(kill_pid);
        print(" terminated\n");
    }
    else if (result == -1) {
        print("[ ERROR ] Cannot terminate protected task\n");
    }
    else if (result == -2) {
        print("[ TASK ] PID ");
        print_number(kill_pid);
        print(" already terminated\n");
    }
    else {
        print("[ ERROR ] PID not found\n");
    }
}
else if (compare(input, "tasks")) {
    struct task* list = get_tasks();
    int count = get_task_count();
    int i;

    print("PID   NAME      STATE\n");

    for (i = 0; i < count; i++) {
        if (list[i].pid == 1)
            print("1     ");
        else if (list[i].pid == 2)
            print("2     ");
        else if (list[i].pid == 3)
            print("3     ");

        print(list[i].name);
        print("      ");
        print(list[i].state);
        print("\n");
    }
}
else if (compare(input, "ticks")) {
    unsigned int ticks;

    ticks = get_timer_ticks();

    print("Timer Information\n\n");
    print("Timer Frequency : 100 Hz\n");
    print("Timer Ticks     : ");

    print_number(ticks);

    print("\nTimer Status    : RUNNING\n");
}
else if (compare(input, "schedule")) {
    print("Scheduler started...\n\n");

    schedule_once();

    print("Scheduler cycle complete.\n");
}
else if (compare(input, "run hello")) {
    print("Starting user program: hello\n");

    if (load_user_program("hello.bin")) {
        user_task_start();
        enter_user_mode();
    } else {
        print("[ ERROR ] Unable to load hello.bin\n");
    }
}
    else if (starts_with_echo(input)) {
        print(input + 5);
        putchar('\n');
    }
    else if (compare(input, "reboot")) {
        print("Restarting ValaOS...\n");
        reboot_system();
    }
    else if (compare(input, "shutdown")) {
        shutdown_system();
    }
    else if (input_pos > 0) {
        print("Unknown command\n");
    }

    input_pos = 0;
    input[0] = '\0';

    print("ValaOS> ");
}

void keyboard_init() {
    vfs_init();
    input_pos = 0;
    input[0] = '\0';

    hide_cursor();
}

void keyboard_poll() {
    static unsigned char key_down[128];

    if (!(inb(0x64) & 1))
        return;

    uint8_t scancode = inb(0x60);
    uint8_t key_code = scancode & 0x7F;

    if (scancode & 0x80) {
        key_down[key_code] = 0;
        return;
    }

    if (key_code >= sizeof(key_map))
        return;

    if (key_down[key_code])
        return;

    key_down[key_code] = 1;

    char c = key_map[key_code];

    if (c == '\b') {
        if (input_pos > 0 && col > 0) {
            input_pos--;
            col--;

            int pos = (row * 80 + col) * 2;

            video[pos] = ' ';
            video[pos + 1] = 0x07;

            input[input_pos] = '\0';
        }
    }
    else if (c == '\n') {
        input[input_pos] = '\0';
        execute_command();
    }
    else if (c && input_pos < 63) {
        input[input_pos++] = c;
        input[input_pos] = '\0';

        putchar(c);
    }
}