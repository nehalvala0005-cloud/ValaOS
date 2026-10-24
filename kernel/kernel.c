/*
 * ValaOS
 * Copyright (c) 2026 Nehal Vala
 * All rights reserved.
 */

extern void vfs_init(void);
extern void keyboard_init();
extern void keyboard_process_pending();
extern void clear_screen();
extern void print(const char *str);

extern void paging_init();
extern int paging_enabled();
extern void enable_write_protection();
extern void interrupts_init();
extern void task_context_init();
extern void tss_init();

extern int map_page_flags(
    unsigned int *virtual_address,
    unsigned int *physical_address,
    unsigned int flags);

extern void enter_user_mode(unsigned int user_esp);
extern void enter_user_program(unsigned int address);
extern int user_task_start();
extern int user_process_abort(unsigned int pid);
extern int vfs_read_file(const char *name, char *buffer, int max_size);
extern int user_process_copy_memory(
    const void *code_source,
    const void *stack_source,
    unsigned int user_esp);

#define PAGE_PRESENT 0x001
#define PAGE_WRITABLE 0x002
#define PAGE_USER 0x004

#define MAX_COMMAND_ARGS 8
#define MAX_COMMAND_ARG_LEN 64

#define USER_STACK_BASE 0x00800000
#define USER_STACK_SIZE 4096

static unsigned char user_code_page[4096]
    __attribute__((aligned(4096)));

static unsigned char user_test_page[4096]
    __attribute__((aligned(4096)));

static unsigned char user_stack_page[4096]
    __attribute__((aligned(4096)));

static int hex_value(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;

    return -1;
}

int load_user_program(const char *name)
{
    static char hex_data[256];
    int length;
    int i;
    int high;
    int low;
    int byte_count;

    length = vfs_read_file(name, hex_data, sizeof(hex_data));

    if (length <= 0)
        return 0;

    if (length % 2 != 0)
        return 0;

    byte_count = length / 2;

    if (byte_count > 4096)
        return 0;

    for (i = 0; i < 4096; i++)
        user_code_page[i] = 0;

    for (i = 0; i < byte_count; i++)
    {
        high = hex_value(hex_data[i * 2]);
        low = hex_value(hex_data[i * 2 + 1]);

        if (high < 0 || low < 0)
            return 0;

        user_code_page[i] =
            (unsigned char)((high << 4) | low);
    }

    return 1;
}

static int prepare_user_arguments(
    int argc,
    char args[][MAX_COMMAND_ARG_LEN],
    unsigned int *user_esp)
{
    unsigned int stack_pos;
    unsigned int string_pos;
    unsigned int string_addresses[MAX_COMMAND_ARGS];
    unsigned int *stack;
    int i;
    int j;
    int length;

    if (argc <= 0 || argc > MAX_COMMAND_ARGS)
        return 0;

    for (i = 0; i < USER_STACK_SIZE; i++)
        user_stack_page[i] = 0;

    stack_pos = USER_STACK_SIZE;
    string_pos = USER_STACK_SIZE;

    for (i = argc - 1; i >= 0; i--)
    {
        length = 0;

        while (args[i][length] != '\0' &&
               length < MAX_COMMAND_ARG_LEN - 1)
            length++;

        if (string_pos < (unsigned int)(length + 1))
            return 0;

        string_pos -= length + 1;

        for (j = 0; j < length; j++)
            user_stack_page[string_pos + j] = args[i][j];

        user_stack_page[string_pos + length] = '\0';

        string_addresses[i] = USER_STACK_BASE + string_pos;
    }

    stack_pos = string_pos;
    stack_pos &= ~3;

    if (stack_pos < (unsigned int)((argc + 2) * 4))
        return 0;

    stack_pos -= (argc + 2) * 4;

    stack = (unsigned int *)(user_stack_page + stack_pos);

    stack[0] = argc;

    for (i = 0; i < argc; i++)
        stack[i + 1] = string_addresses[i];

    stack[argc + 1] = 0;

    *user_esp = USER_STACK_BASE + stack_pos;

    return 1;
}

int load_user_program_args(
    const char *name,
    int argc,
    char args[][MAX_COMMAND_ARG_LEN],
    unsigned int *user_esp)
{
    if (!load_user_program(name))
        return 0;

    if (!prepare_user_arguments(argc, args, user_esp))
        return 0;

    return 1;
}

int prepare_user_process_memory(unsigned int user_esp)
{
    return user_process_copy_memory(
        user_code_page,
        user_stack_page,
        user_esp);
}

void user_mode_init()
{

    user_code_page[0] = 0xB8;
    user_code_page[1] = 0x01;
    user_code_page[2] = 0x00;
    user_code_page[3] = 0x00;
    user_code_page[4] = 0x00;

    user_code_page[5] = 0xBB;
    user_code_page[6] = 'U';
    user_code_page[7] = 0x00;
    user_code_page[8] = 0x00;
    user_code_page[9] = 0x00;

    user_code_page[10] = 0xCD;
    user_code_page[11] = 0x80;

    user_code_page[12] = 0xB8;
    user_code_page[13] = 0x02;
    user_code_page[14] = 0x00;
    user_code_page[15] = 0x00;
    user_code_page[16] = 0x00;

    user_code_page[17] = 0x31;
    user_code_page[18] = 0xDB;

    user_code_page[19] = 0xCD;
    user_code_page[20] = 0x80;

    user_code_page[21] = 0xEB;
    user_code_page[22] = 0xFE;

    user_test_page[0] = 0xB8;
    user_test_page[1] = 0x01;
    user_test_page[2] = 0x00;
    user_test_page[3] = 0x00;
    user_test_page[4] = 0x00;

    user_test_page[5] = 0xBB;
    user_test_page[6] = 'T';
    user_test_page[7] = 0x00;
    user_test_page[8] = 0x00;
    user_test_page[9] = 0x00;

    user_test_page[10] = 0xCD;
    user_test_page[11] = 0x80;

    user_test_page[12] = 0xB8;
    user_test_page[13] = 0x02;
    user_test_page[14] = 0x00;
    user_test_page[15] = 0x00;
    user_test_page[16] = 0x00;

    user_test_page[17] = 0x31;
    user_test_page[18] = 0xDB;

    user_test_page[19] = 0xCD;
    user_test_page[20] = 0x80;

    user_test_page[21] = 0xEB;
    user_test_page[22] = 0xFE;
}

extern int user_process_start(const char *name);
extern int task_kill(unsigned int pid);

int run_syscall_reliability_test(void)
{

    static const unsigned char test_code[] = {
        0xB8, 0x01, 0x00, 0x00, 0x00,
        0xBB, 0x41, 0x00, 0x00, 0x00,
        0xCD, 0x80,

        0xB8, 0x63, 0x00, 0x00, 0x00,
        0x31, 0xDB,
        0xCD, 0x80,
        0x3D, 0xFF, 0xFF, 0xFF, 0xFF,
        0x75, 0x15,

        0xB8, 0x01, 0x00, 0x00, 0x00,
        0xBB, 0x42, 0x00, 0x00, 0x00,
        0xCD, 0x80,

        0xB8, 0x02, 0x00, 0x00, 0x00,
        0x31, 0xDB,
        0xCD, 0x80,

        0xB8, 0x01, 0x00, 0x00, 0x00,
        0xBB, 0x46, 0x00, 0x00, 0x00,
        0xCD, 0x80,

        0xB8, 0x02, 0x00, 0x00, 0x00,
        0x31, 0xDB,
        0xCD, 0x80};

    unsigned int i;
    int pid;

    for (i = 0; i < sizeof(user_code_page); i++)
    {
        user_code_page[i] = 0;
        user_stack_page[i] = 0;
    }

    for (i = 0; i < sizeof(test_code); i++)
        user_code_page[i] = test_code[i];

    pid = user_process_start("syscalltest");

    if (pid < 0)
        return 0;

    if (!user_process_copy_memory(
            user_code_page,
            user_stack_page,
            USER_STACK_BASE + USER_STACK_SIZE))
    {
        user_process_abort((unsigned int)pid);
        return 0;
    }

    if (!user_task_start())
    {
        user_process_abort((unsigned int)pid);
        return 0;
    }
    enter_user_mode(USER_STACK_BASE + USER_STACK_SIZE);

    return 1;
}

void kernel_main()
{

    __asm__ volatile("cli");

    clear_screen();

    paging_init();
    enable_write_protection();
    interrupts_init();
    tss_init();
    task_context_init();
    user_mode_init();

    print("========================================\n");
    print("              V A L A O S\n");
    print("========================================\n\n");

    print("ValaOS Kernel v4.0\n\n");

    print("[ OK ] Kernel loaded\n");
    print("[ OK ] VGA initialized\n");
    print("[ OK ] Keyboard driver initialized\n");
    print("[ OK ] Command shell initialized\n\n");

    print("Welcome to ValaOS!\n\n");

    vfs_init();
    keyboard_init();
    print("ValaOS> ");
    __asm__ volatile("sti");

    while (1)
    {
        keyboard_process_pending();
    }
}
