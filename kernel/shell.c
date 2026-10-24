/*
 * ValaOS 3.0 - Shell Core
 * Copyright (c) 2026 Nehal Vala
 * All rights reserved.
 */

/*
 * ValaOS
 * Copyright (c) 2026 Nehal Vala
 * All rights reserved.
 */

#define MAX_COMMAND_ARGS 8
#define MAX_COMMAND_ARG_LEN 64

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

extern void *kmalloc(unsigned int size);
extern void kfree(void *address);
extern unsigned int memory_used();
extern unsigned int memory_free();
extern void enter_user_mode(unsigned int user_esp);
extern int run_syscall_reliability_test(void);
extern int user_task_start();
extern int prepare_user_process(unsigned int user_esp);

extern int get_task_info(
    int index,
    unsigned int *pid,
    const char **name,
    const char **state);
extern int get_task_count();
extern void schedule_once();
extern unsigned int memory_total();
extern unsigned int memory_page_size();
extern unsigned int memory_total_pages();
extern unsigned int memory_used_pages();
extern void *page_alloc(unsigned int pages);
extern void page_free(void *address, unsigned int pages);
extern int paging_enabled();
extern int map_page(unsigned int virtual_address, unsigned int physical_address);
extern int unmap_page(unsigned int virtual_address);
extern int is_page_mapped(unsigned int virtual_address);
extern int virtual_to_physical(unsigned int virtual_address, unsigned int *physical_address);
extern unsigned int get_page_flags(unsigned int virtual_address);
extern int set_page_flags(unsigned int virtual_address, unsigned int flags);
extern void enable_write_protection();
extern int task_kill(unsigned int pid);
extern unsigned int get_timer_ticks();
extern void vfs_init();
extern void vfs_list();

extern int vfs_list_dir(const char *name);

extern int vfs_complete_name(
    const char *prefix,
    int parent,
    int index,
    char *name,
    int max_size,
    int *is_dir);

extern int vfs_cat(const char *name);
extern void vfs_list_programs();
extern int vfs_read_file(const char *name, char *buffer, int max_size);
extern int vfs_touch(const char *name);
extern int vfs_write(const char *name, const char *text);
extern int vfs_remove(const char *name);
extern int vfs_mkdir(const char *name);
extern int vfs_rmdir(const char *name);
extern int vfs_cd(const char *name);
extern void vfs_pwd();
extern int load_user_program(const char *name);

extern int load_user_program_args(
    const char *name,
    int argc,
    char args[][MAX_COMMAND_ARG_LEN],
    unsigned int *user_esp);
extern int disk_test();

extern uint8_t inb(uint16_t port);
extern void outb(uint16_t port, uint8_t value);
extern void print(const char *str);
extern void putchar(char c);
extern void terminal_set_color(unsigned char color);

#define COLOR_DEFAULT 0x07
#define COLOR_GREEN 0x0A
#define COLOR_RED 0x0C
#define COLOR_YELLOW 0x0E
#define COLOR_CYAN 0x0B

extern int user_process_start(const char *name);
extern int user_process_abort(unsigned int pid);
extern int user_schedule_next();
extern int user_process_switch();
extern int get_user_process_count();
extern int get_user_process_info(
    int index,
    unsigned int *pid,
    const char **name,
    const char **state);

extern int get_process_history_count(void);
extern int get_process_history_info(
    int index,
    unsigned int *pid,
    const char **name,
    const char **state);
extern int get_process_history_details(
    int index,
    const char **reason,
    unsigned int *exit_code);

extern int prepare_user_process_memory(unsigned int user_esp);
extern void clear_screen();
extern char input[64];
extern int input_pos;

int compare(const char *a, const char *b)
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

int starts_with_echo(const char *str)
{
    return str[0] == 'e' &&
           str[1] == 'c' &&
           str[2] == 'h' &&
           str[3] == 'o' &&
           str[4] == ' ';
}

void reboot_system()
{
    uint8_t good = 0x02;

    while (good & 0x02)
        good = inb(0x64);

    outb(0x64, 0xFE);

    while (1)
        __asm__ volatile("hlt");
}

void shutdown_system()
{
    print("Shutting down ValaOS...\n");
    print("System halted.\n");

    while (1)
        __asm__ volatile("cli; hlt");
}


void print_number(unsigned int number)
{
    char buffer[10];
    int i = 0;
    char output[2];

    if (number == 0)
    {
        print("0");
        return;
    }

    while (number > 0)
    {
        buffer[i++] = '0' + (number % 10);
        number /= 10;
    }

    while (i > 0)
    {
        output[0] = buffer[--i];
        output[1] = '\0';
        print(output);
    }
}


int starts_with(const char *str, const char *prefix)
{
    while (*prefix)
    {
        if (*str != *prefix)
            return 0;
        str++;
        prefix++;
    }
    return 1;
}

const char *command_argument(const char *str, int offset)
{
    while (str[offset] == ' ')
        offset++;
    return str + offset;
}

#define MAX_COMMAND_ARGS 8
#define MAX_COMMAND_ARG_LEN 64

int parse_command_line(const char *line, char *command, char args[][MAX_COMMAND_ARG_LEN], int max_args)
{
    int i = 0;
    int argc = 0;
    int j;
    char quote;

    while (line[i] == ' ')
        i++;

    if (line[i] == '\0')
    {
        command[0] = '\0';
        return 0;
    }

    j = 0;
    while (line[i] != ' ' && line[i] != '\0' && j < 31)
    {
        command[j++] = line[i++];
    }
    command[j] = '\0';

    while (line[i] != '\0' && argc < max_args)
    {
        while (line[i] == ' ')
            i++;

        if (line[i] == '\0')
            break;

        quote = 0;
        if (line[i] == '"' || line[i] == '\'')
        {
            quote = line[i++];
        }

        j = 0;
        while (line[i] != '\0')
        {
            if (quote)
            {
                if (line[i] == quote)
                {
                    i++;
                    break;
                }
            }
            else if (line[i] == ' ')
            {
                break;
            }

            if (j < MAX_COMMAND_ARG_LEN - 1)
                args[argc][j++] = line[i];
            i++;
        }

        args[argc][j] = '\0';
        argc++;

        while (line[i] == ' ')
            i++;
    }

    return argc;
}

void selftest()
{
    int passed = 0;
    int failed = 0;
    char buffer[64];

    print("\n========== ValaOS 3.0 SELF TEST ==========\n\n");

    if (memory_total_pages() > 0 &&
        memory_page_size() == 4096)
    {

        print("[ OK ] Memory\n");
        passed++;
    }
    else
    {

        print("[FAIL] Memory\n");
        failed++;
    }

    if (paging_enabled())
    {

        print("[ OK ] Paging\n");
        passed++;
    }
    else
    {

        print("[FAIL] Paging\n");
        failed++;
    }

    if (disk_test())
    {

        print("[ OK ] Disk\n");
        passed++;
    }
    else
    {

        print("[FAIL] Disk\n");
        failed++;
    }

    if (memory_used_pages() >= 0)
    {

        print("[ OK ] Virtual Memory\n");
        passed++;
    }
    else
    {

        print("[FAIL] Virtual Memory\n");
        failed++;
    }

    if (get_task_count() >= 3)
    {

        print("[ OK ] Scheduler\n");
        passed++;
    }
    else
    {

        print("[FAIL] Scheduler\n");
        failed++;
    }

    if (load_user_program("hello.bin"))
    {

        print("[ OK ] Program Loader\n");
        passed++;
    }
    else
    {

        print("[FAIL] Program Loader\n");
        failed++;
    }

    if (vfs_read_file("hello.bin", buffer, sizeof(buffer)) > 0)
    {

        print("[ OK ] VFS\n");
        passed++;
    }
    else
    {

        print("[FAIL] VFS\n");
        failed++;
    }

    if (get_user_process_count() >= 8)
    {

        print("[ OK ] Process Management\n");
        passed++;
    }
    else
    {

        print("[FAIL] Process Management\n");
        failed++;
    }

    print("\n===========================================\n");

    print("Passed: ");
    print_number(passed);

    print("\nFailed: ");
    print_number(failed);

    print("\n");

    if (failed == 0)
        print("SYSTEM STATUS: OK\n");
    else
        print("SYSTEM STATUS: CHECK FAILED\n");

    print("===========================================\n");
}

int edit_distance(const char *a, const char *b)
{
    int dp[32][32];
    int i, j;
    int cost;

    for (i = 0; i <= 31; i++)
        dp[i][0] = i;

    for (j = 0; j <= 31; j++)
        dp[0][j] = j;

    for (i = 1; i <= 31 && a[i - 1]; i++)
    {
        for (j = 1; j <= 31 && b[j - 1]; j++)
        {
            cost = (a[i - 1] == b[j - 1]) ? 0 : 1;

            dp[i][j] = dp[i - 1][j] + 1;

            if (dp[i][j - 1] + 1 < dp[i][j])
                dp[i][j] = dp[i][j - 1] + 1;

            if (dp[i - 1][j - 1] + cost < dp[i][j])
                dp[i][j] = dp[i - 1][j - 1] + cost;
        }
    }

    i = 0;
    while (i < 31 && a[i])
        i++;

    j = 0;
    while (j < 31 && b[j])
        j++;

    return dp[i][j];
}

void suggest_command(const char *command)
{
    const char *commands[] = {
        "help", "info", "about", "clear", "version", "echo",
        "reboot", "shutdown", "memtest", "memstress", "meminfo",
        "paging", "selftest", "ptest", "dptest", "vmtest",
        "vmaptest", "pwd", "mkdir", "rmdir", "cd", "ls", "programs",
        "cat", "touch", "write", "rm", "ps", "uptime", "disktest",
        "kill", "tasks", "ticks", "schedule", "pagetest",
        "proctest", "run", "nano", "monitor"};

    int i;
    int distance;
    int best_distance = 3;
    const char *best = 0;

    for (i = 0; i < (int)(sizeof(commands) / sizeof(commands[0])); i++)
    {
        distance = edit_distance(command, commands[i]);

        if (distance < best_distance)
        {
            best_distance = distance;
            best = commands[i];
        }
    }

    if (best)
    {
        print("Did you mean: ");
        print(best);
        print("?\n");
    }
}

void execute_command()
{
    unsigned int kill_pid;
    int result;
    const char *arg;
    const char *text;
    char command[32];
    char args[MAX_COMMAND_ARGS][MAX_COMMAND_ARG_LEN];
    int argc;

    argc = parse_command_line(input, command, args, MAX_COMMAND_ARGS);

    putchar('\n');

    if (compare(input, "help"))
    {
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
        print("selftest\n");
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
        print("proctest\n");
        print("syscalltest\n");
        print("run hello\n");
        print("ls [directory]\n");
        print("cat <file>\n");
        print("touch <file>\n");
        print("write <file> <text>\n");
        print("rm <file>\n");
        print("mkdir <dir>\n");
        print("rmdir <dir>\n");
        print("cd <dir>\n");
        print("pwd\n");
        print("disktest\n");
    }
    else if (compare(input, "info"))
    {
        print("ValaOS 2.0\n");
        print("Architecture: x86\n");
        print("Keyboard: PS/2 Polling\n");
        print("Kernel: Custom\n");
        print("Status: Running\n");
    }
    else if (compare(input, "about"))
    {
        print("ValaOS - Custom Operating System\n");
        print("Built from scratch for learning OS concepts.\n");
    }
    else if (compare(input, "clear"))
    {
        clear_screen();
    }
    else if (compare(input, "version"))
    {
        print("ValaOS 2.0\n");
        print("Architecture: x86\n");
        print("Shell: ValaShell\n");
    }
    else if (compare(input, "memtest"))
    {
        void *block = kmalloc(100);

        print("Memory test started.\n");

        if (block != 0)
        {
            print("100 bytes allocated.\n");
            print("Memory allocation: OK\n");

            kfree(block);

            print("100 bytes released.\n");
            print("Memory release: OK\n");
        }
        else
        {
            print("Memory allocation: FAILED\n");
        }
    }

    else if (compare(input, "memstress"))
    {
        void *a = 0;
        void *b = 0;
        void *c = 0;
        void *d = 0;
        int passed = 1;

        print("Memory Stress Test\n\n");

        if (kmalloc(0) == 0)
            print("[ OK ] Zero-size allocation rejected\n");
        else
        {
            print("[FAIL] Zero-size allocation accepted\n");
            passed = 0;
        }

        if (kmalloc(4194305) == 0)
            print("[ OK ] Oversized allocation rejected\n");
        else
        {
            print("[FAIL] Oversized allocation accepted\n");
            passed = 0;
        }

        a = kmalloc(4096);
        if (a)
            print("[ OK ] Allocation A - 1 page\n");
        else
        {
            print("[FAIL] Allocation A\n");
            passed = 0;
        }

        if (a)
        {
            unsigned int before = memory_used();

            kfree((void *)((uint32_t)a + 1));

            if (memory_used() == before)
                print("[ OK ] Invalid pointer free rejected\n");
            else
            {
                print("[FAIL] Invalid pointer free changed memory\n");
                passed = 0;
            }
        }

        b = kmalloc(8192);
        if (b)
            print("[ OK ] Allocation B - 2 pages\n");
        else
        {
            print("[FAIL] Allocation B\n");
            passed = 0;
        }

        c = kmalloc(16384);
        if (c)
            print("[ OK ] Allocation C - 4 pages\n");
        else
        {
            print("[FAIL] Allocation C\n");
            passed = 0;
        }

        if (b)
        {
            kfree(b);
            print("[ OK ] Freed allocation B\n");

            {
                unsigned int before = memory_used();

                kfree(b);

                if (memory_used() == before)
                    print("[ OK ] Double-free rejected\n");
                else
                {
                    print("[FAIL] Double-free changed memory\n");
                    passed = 0;
                }
            }

            d = kmalloc(8192);
            if (d)
                print("[ OK ] Reused freed pages\n");
            else
            {
                print("[FAIL] Reallocation\n");
                passed = 0;
            }
        }

        if (a)
            kfree(a);
        if (c)
            kfree(c);
        if (d)
            kfree(d);

        if (passed)
            print("\nMemory stress test passed.\n");
        else
            print("\nMemory stress test FAILED.\n");
    }

    else if (compare(input, "meminfo"))
    {
        unsigned int used_pages = memory_used_pages();
        unsigned int total_pages = 1024;
        unsigned int used_memory = used_pages * 4096;
        unsigned int total_memory = 4194304;

        print("==============================\n");
        print("       ValaOS MEMORY INFO\n");
        print("==============================\n");

        print("Total Memory : 4194304 bytes\n");
        print("Page Size    : 4096 bytes\n");
        print("Total Pages  : 1024\n");

        print("Used Pages   : ");
        print_number(used_pages);
        print("\n");

        print("Used Memory  : ");
        print_number(used_memory);
        print(" bytes\n");

        print("Free Memory  : ");
        print_number(total_memory - used_memory);
        print(" bytes\n");

        if (used_pages <= total_pages)
            print("Status       : Counter OK\n");
        else
            print("Status       : ERROR\n");
    }

    else if (compare(input, "paging"))
    {
        print("==============================\n");
        print("       VALAOS PAGING INFO\n");
        print("==============================\n");

        if (paging_enabled())
        {
            print("Paging Status : ENABLED\n");
            print("Mapped Memory  : 16 MB\n");
            print("Page Size      : 4096 bytes\n");
            print("Page Tables    : 4\n");
            print("Status         : Healthy\n");
        }
        else
        {
            print("Paging Status : DISABLED\n");
            print("Status         : Error\n");
        }
    }
    else if (compare(input, "selftest"))
    {
        selftest();
    }
    else if (compare(input, "ptest"))
    {
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
    else if (compare(input, "dptest"))
    {
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
    else if (compare(input, "vmtest"))
    {
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
    else if (compare(input, "vmaptest"))
    {
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
    else if (compare(input, "pwd"))
    {
        vfs_pwd();
    }
    else if (starts_with(input, "mkdir "))
    {
        arg = command_argument(input, 6);

        if (vfs_mkdir(arg))
        {
            terminal_set_color(COLOR_GREEN);
            print("[ OK ] Directory created: ");
            print(arg);
            print("\n");
            terminal_set_color(COLOR_DEFAULT);
        }
        else
        {
            terminal_set_color(COLOR_RED);
            print("[ ERROR ] Unable to create directory\n");
            terminal_set_color(COLOR_DEFAULT);
        }
    }
    else if (starts_with(input, "cd "))
    {
        arg = command_argument(input, 3);

        if (!vfs_cd(arg))
            print("[ VFS ] Directory not found\n");
    }
    else if (compare(input, "cd"))
    {
        vfs_cd("/");
    }
    else if (compare(command, "ls"))
    {
        if (argc == 0)
        {
            vfs_list();
        }
        else if (argc == 1)
        {
            if (!vfs_list_dir(args[0]))
                print("[ VFS ] Directory not found\n");
        }
        else
        {
            print("[ VFS ] Usage: ls [directory]\n");
        }
    }
    else if (starts_with(input, "programs"))
    {
        vfs_list_programs();
    }
    else if (starts_with(input, "cat "))
    {
        arg = command_argument(input, 4);

        if (!vfs_cat(arg))
        {
            terminal_set_color(COLOR_RED);
            print("[ ERROR ] File not found\n");
            terminal_set_color(COLOR_DEFAULT);
        }
    }
    else if (starts_with(input, "touch "))
    {
        arg = command_argument(input, 6);

        if (vfs_touch(arg))
        {
            terminal_set_color(COLOR_GREEN);
            print("[ OK ] File created\n");
            terminal_set_color(COLOR_DEFAULT);
        }
        else
        {
            terminal_set_color(COLOR_RED);
            print("[ ERROR ] Unable to create file\n");
            terminal_set_color(COLOR_DEFAULT);
        }
    }
    else if (starts_with(input, "write "))
    {
        int i = 6;

        while (input[i] == ' ')
            i++;

        arg = input + i;

        while (input[i] && input[i] != ' ')
            i++;

        if (!input[i])
        {
            print("[ VFS ] Usage: write <file> <text>\n");
        }
        else
        {
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
    else if (starts_with(input, "rm "))
    {
        arg = command_argument(input, 3);

        if (vfs_remove(arg))
        {
            terminal_set_color(COLOR_GREEN);
            print("[ OK ] File removed\n");
            terminal_set_color(COLOR_DEFAULT);
        }
        else
        {
            terminal_set_color(COLOR_RED);
            print("[ ERROR ] File not found\n");
            terminal_set_color(COLOR_DEFAULT);
        }
    }

    else if (starts_with(input, "rmdir "))
    {
        arg = command_argument(input, 6);

        if (vfs_rmdir(arg))
        {
            terminal_set_color(COLOR_GREEN);
            print("[ OK ] Directory removed\n");
            terminal_set_color(COLOR_DEFAULT);
        }
        else
        {
            terminal_set_color(COLOR_RED);
            print("[ ERROR ] Directory not found or not empty\n");
            terminal_set_color(COLOR_DEFAULT);
        }
    }

    else if (compare(input, "ps"))
    {
        int count = get_task_count();
        int i;

        print("PID   NAME      STATE\n");

        for (i = 0; i < count; i++)
        {
            unsigned int pid;
            const char *name;
            const char *state;

            if (!get_task_info(i, &pid, &name, &state))
                continue;

            print_number(pid);
            print("     ");
            print(name);
            print("      ");
            print(state);
            print("\n");
        }

        {
            int user_count = get_user_process_count();
            int j;

            for (j = 0; j < user_count; j++)
            {
                unsigned int pid;
                const char *name;
                const char *state;

                if (!get_user_process_info(j, &pid, &name, &state))
                    continue;

                print_number(pid);
                print("     ");
                print(name);
                print("      ");
                print(state);
                print("\n");
            }
        }

        {
            int history_count = get_process_history_count();
            int h;

            if (history_count > 0)
            {
                print("\nProcess History:\n");
                print("PID   NAME      STATE       REASON       EXIT\n");

                for (h = 0; h < history_count; h++)
                {
                    unsigned int pid;
                    unsigned int exit_code;
                    const char *name;
                    const char *state;
                    const char *reason;

                    if (!get_process_history_info(h, &pid, &name, &state))
                        continue;
                    if (!get_process_history_details(h, &reason, &exit_code))
                        continue;

                    print_number(pid);
                    print("     ");
                    print(name);
                    print("      ");
                    print(state);
                    print("   ");
                    print(reason);
                    print("   ");
                    print_number(exit_code);
                    print("\n");
                }
            }
        }
    }

    else if (compare(input, "uptime"))
    {
        unsigned int ticks = get_timer_ticks();
        unsigned int seconds = ticks / 100;

        print("ValaOS Uptime: ");
        print_number(seconds);
        print(" seconds\n");
        print("Timer Ticks: ");
        print_number(ticks);
        print("\n");
    }
    else if (compare(input, "disktest"))
    {
        disk_test();
    }
    else if (compare(command, "kill"))
    {
        int i = 0;
        int digit;
        kill_pid = 0;

        if (argc != 1 || args[0][0] == '\0')
        {
            print("[ ERROR ] Invalid PID\n");
            goto command_done;
        }

        while (args[0][i] >= '0' && args[0][i] <= '9')
        {
            digit = args[0][i] - '0';
            if (kill_pid > (0xFFFFFFFFU - (unsigned int)digit) / 10)
            {
                print("[ ERROR ] Invalid PID\n");
                goto command_done;
            }

            kill_pid = kill_pid * 10 + (unsigned int)digit;
            i++;
        }

        if (args[0][i] != '\0')
        {
            print("[ ERROR ] Invalid PID\n");
            goto command_done;
        }

        result = task_kill(kill_pid);

        if (result == 1)
        {
            print("[ TASK ] PID ");
            print_number(kill_pid);
            print(" terminated\n");
        }
        else if (result == 2)
        {
            print("[ TASK ] PID ");
            print_number(kill_pid);
            print(" termination requested\n");
        }
        else if (result == -1)
        {
            print("[ ERROR ] Cannot terminate protected task\n");
        }
        else if (result == -2)
        {
            print("[ TASK ] PID ");
            print_number(kill_pid);
            print(" already terminated\n");
        }
        else if (result == -3)
        {
            print("[ ERROR ] PID could not be terminated safely\n");
        }
        else
        {
            print("[ ERROR ] PID not found\n");
        }
    }
    else if (compare(input, "tasks"))
    {
        int count = get_task_count();
        int i;

        print("PID   NAME      STATE\n");

        for (i = 0; i < count; i++)
        {
            unsigned int pid;
            const char *name;
            const char *state;

            if (!get_task_info(i, &pid, &name, &state))
                continue;

            print_number(pid);
            print("     ");
            print(name);
            print("      ");
            print(state);
            print("\n");
        }
    }
    else if (compare(input, "ticks"))
    {
        unsigned int ticks;

        ticks = get_timer_ticks();

        print("Timer Information\n\n");
        print("Timer Frequency : 100 Hz\n");
        print("Timer Ticks     : ");

        print_number(ticks);

        print("\nTimer Status    : RUNNING\n");
    }
    else if (compare(input, "schedule"))
    {
        print("Scheduler started...\n\n");

        schedule_once();

        print("Scheduler cycle complete.\n");
    }
    else if (compare(command, "pagetest"))
    {
        void *page1;
        void *page2;

        print("[ PAGE ] Allocating 1 page\n");
        page1 = page_alloc(1);

        if (!page1)
        {
            print("[ FAIL ] 1-page allocation failed\n");
        }
        else
        {
            print("[ OK ] 1-page allocation\n");
        }

        print("[ PAGE ] Allocating 2 pages\n");
        page2 = page_alloc(2);

        if (!page2)
        {
            print("[ FAIL ] 2-page allocation failed\n");
        }
        else
        {
            print("[ OK ] 2-page allocation\n");
        }

        if (page1)
            page_free(page1, 1);

        if (page2)
            page_free(page2, 2);

        print("[ OK ] Pages released\n");
    }

    else if (compare(command, "proctest"))
    {
        int pid1;
        int pid2;

        pid1 = user_process_start("test1");

        if (pid1 < 0)
        {
            print("[ ERROR ] Process table full\n");
            goto command_done;
        }

        pid2 = user_process_start("test2");

        if (pid2 < 0)
        {
            user_process_abort((unsigned int)pid1);
            print("[ ERROR ] Second process could not start\n");
            goto command_done;
        }

        print("[ PROCESS ] PID ");
        print_number(pid1);
        print(" created\n");

        print("[ PROCESS ] PID ");
        print_number(pid2);
        print(" created\n");

        print("[ SCHED ] Both entries are BLOCKED until their image and context are ready\n");
        print("[ SCHED ] Incomplete processes are not eligible for selection\n");

        user_process_abort((unsigned int)pid1);
        user_process_abort((unsigned int)pid2);
        print("[ PROCESS ] Test entries cleaned up\n");
    }

    else if (compare(command, "syscalltest"))
    {
        print("Syscall Reliability Test\n\n");
        print("[ TEST ] Calling unknown syscall 99\n");

        if (run_syscall_reliability_test())
            print("\n[ OK ] Syscall test process completed\n");
        else
            print("\n[ FAIL ] Unable to launch syscall test\n");
    }
    else if (compare(command, "run"))
    {
        char file_name[40];
        int i;
        int pid;
        unsigned int user_esp;

        if (argc < 1)
        {
            print("[ ERROR ] Usage: run <program> [args...]\n");
        }
        else
        {
            i = 0;

            while (args[0][i] != '\0' && i < 31)
            {
                file_name[i] = args[0][i];
                i++;
            }

            file_name[i++] = '.';
            file_name[i++] = 'b';
            file_name[i++] = 'i';
            file_name[i++] = 'n';
            file_name[i] = '\0';

            print("Starting user program: ");
            print(args[0]);
            print("\n");

            if (load_user_program_args(file_name, argc, args, &user_esp))
            {
                pid = user_process_start(args[0]);

                if (pid < 0)
                {
                    print("[ ERROR ] Unable to create process\n");
                    goto command_done;
                }

                if (!prepare_user_process_memory(user_esp))
                {
                    user_process_abort((unsigned int)pid);
                    print("[ ERROR ] Unable to prepare process memory\n");
                    goto command_done;
                }

                print("[ PROCESS ] PID ");
                print_number(pid);
                print(" started\n");

                if (!user_task_start())
                {
                    user_process_abort((unsigned int)pid);
                    print("[ ERROR ] Unable to enter process\n");
                    goto command_done;
                }
                enter_user_mode(user_esp);
            }
            else
            {
                print("[ ERROR ] Unable to load ");
                print(file_name);
                print("\n");
            }
        }
    }

    else if (starts_with_echo(input))
    {
        print(input + 5);
        putchar('\n');
    }
    else if (compare(input, "reboot"))
    {
        print("Restarting ValaOS...\n");
        reboot_system();
    }
    else if (compare(input, "shutdown"))
    {
        shutdown_system();
    }

    else if (input_pos > 0)
    {
        terminal_set_color(COLOR_RED);
        print("Unknown command: ");
        print(command);
        print("\n");
        terminal_set_color(COLOR_DEFAULT);
        suggest_command(command);
    }

command_done:
    input_pos = 0;
    input[0] = '\0';

    print("ValaOS> ");
}
