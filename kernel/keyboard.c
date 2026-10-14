
/*
 * ValaOS
 * Copyright (c) 2026 Nehal Vala
 * All rights reserved.
 */

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

struct task
{
    uint32_t pid;
    const char *name;
    const char *state;
    uint32_t stack_base;
    uint32_t stack_top;
    struct
    {
        uint32_t edi;
        uint32_t esi;
        uint32_t ebp;
        uint32_t esp;
        uint32_t ebx;
        uint32_t edx;
        uint32_t ecx;
        uint32_t eax;
        uint32_t eip;
        uint32_t cs;
        uint32_t eflags;
    } context;
};

extern void *kmalloc(unsigned int size);
extern void kfree(void *address);
extern unsigned int memory_used();
extern unsigned int memory_free();
extern void enter_user_mode();
extern void user_task_start();

extern struct task *get_tasks();
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
extern int virtual_to_physical(unsigned int virtual_address, unsigned int *physical_address);
extern unsigned int get_page_flags(unsigned int virtual_address);
extern int set_page_flags(unsigned int virtual_address, unsigned int flags);
extern void enable_write_protection();
extern int task_kill(unsigned int pid);
extern unsigned int get_timer_ticks();
extern void vfs_init();
extern void vfs_list();
extern int vfs_cat(const char *name);
extern void vfs_list_programs();
extern int vfs_read_file(const char *name, char *buffer, int max_size);
extern int vfs_touch(const char *name);
extern int vfs_write(const char *name, const char *text);
extern int vfs_remove(const char *name);
extern int vfs_mkdir(const char *name);
extern int vfs_cd(const char *name);
extern void vfs_pwd();
extern int load_user_program(const char *name);
extern int disk_test();

unsigned char *video = (unsigned char *)0xB8000;

char input[64];
int input_pos = 0;

int row = 0;
int col = 0;

char key_map[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    0, 0, ' '};

char shift_key_map[] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    0, 0, ' '};

uint8_t inb(uint16_t port)
{
    uint8_t value;

    __asm__ volatile(
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port));

    return value;
}

void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile(
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port));
}

void hide_cursor()
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void move_cursor_next_line()
{
    col = 0;
    row++;

    if (row >= 25)
    {
        for (int r = 1; r < 25; r++)
        {
            for (int c = 0; c < 80; c++)
            {
                int from = (r * 80 + c) * 2;
                int to = ((r - 1) * 80 + c) * 2;

                video[to] = video[from];
                video[to + 1] = video[from + 1];
            }
        }

        for (int c = 0; c < 80; c++)
        {
            int pos = (24 * 80 + c) * 2;

            video[pos] = ' ';
            video[pos + 1] = 0x07;
        }

        row = 24;
    }
}

void putchar(char c)
{
    if (c == '\n')
    {
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

void print(const char *str)
{
    while (*str)
    {
        putchar(*str);
        str++;
    }
}

void clear_screen()
{
    for (int r = 0; r < 25; r++)
    {
        for (int c = 0; c < 80; c++)
        {
            int pos = (r * 80 + c) * 2;

            video[pos] = ' ';
            video[pos + 1] = 0x07;
        }
    }

    row = 0;
    col = 0;
}

extern void execute_command();

#define HISTORY_SIZE 10

static int shift_pressed = 0;
static int caps_lock = 0;
static unsigned char key_down[128];
static volatile int command_pending = 0;

static char command_history[HISTORY_SIZE][64];
static int history_count = 0;
static int history_index = 0;

static int input_cursor = 0;
static int extended_scancode = 0;

static int input_start_row = 0;
static int input_start_col = 0;
static int input_display_len = 0;

static void redraw_input()
{
    int start = input_start_row * 80 + input_start_col;
    int clear_length = input_display_len;

    if (input_pos > clear_length)
        clear_length = input_pos;

    for (int i = 0; i < clear_length; i++)
    {
        int cell = start + i;

        if (cell >= 0 && cell < 80 * 25)
        {
            int pos = cell * 2;
            video[pos] = ' ';
            video[pos + 1] = 0x07;
        }
    }

    for (int i = 0; i < input_pos; i++)
    {
        int cell = start + i;

        if (cell >= 0 && cell < 80 * 25)
        {
            int pos = cell * 2;
            video[pos] = input[i];
            video[pos + 1] = 0x07;
        }
    }

    input_display_len = input_pos;

    int end = start + input_pos;

    if (end >= 80 * 25)
        end = 80 * 25 - 1;

    row = end / 80;
    col = end % 80;
}

static void move_input_cursor(int direction)
{
    if (direction < 0 && input_cursor > 0)
        input_cursor--;

    if (direction > 0 && input_cursor < input_pos)
        input_cursor++;

    hide_cursor();
}

static void load_history_command(int index)
{
    if (index < 0 || index >= history_count)
        return;

    int i = 0;

    while (i < 63 && command_history[index][i])
    {
        input[i] = command_history[index][i];
        i++;
    }

    input[i] = '\0';
    input_pos = i;
    input_cursor = input_pos;

    redraw_input();
}

static void save_history_command()
{
    if (input_pos == 0)
        return;

    if (history_count > 0)
    {
        int same = 1;
        int i = 0;

        while (input[i] && command_history[history_count - 1][i])
        {
            if (input[i] != command_history[history_count - 1][i])
            {
                same = 0;
                break;
            }

            i++;
        }

        if (same && input[i] == '\0' &&
            command_history[history_count - 1][i] == '\0')
            return;
    }

    if (history_count == HISTORY_SIZE)
    {
        for (int i = 1; i < HISTORY_SIZE; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                command_history[i - 1][j] = command_history[i][j];

                if (command_history[i][j] == '\0')
                    break;
            }
        }

        history_count--;
    }

    int i = 0;

    while (i < 63 && input[i])
    {
        command_history[history_count][i] = input[i];
        i++;
    }

    command_history[history_count][i] = '\0';
    history_count++;
}

void keyboard_init()
{
    vfs_init();

    input_pos = 0;
    input_cursor = 0;
    input[0] = '\0';

    history_index = history_count;

    input_start_row = row;
    input_start_col = col;
    input_display_len = 0;

    extended_scancode = 0;
    command_pending = 0;

    hide_cursor();
}

int starts_with(const char *str, const char *prefix);

const char *command_list[] = {
    "help", "info", "about", "clear", "version",
    "echo", "reboot", "shutdown", "memtest",
    "memstress", "meminfo", "paging", "selftest",
    "ptest", "dptest", "vmtest", "vmaptest",
    "pwd", "mkdir", "cd", "ls", "programs",
    "cat", "touch", "write", "rm", "ps",
    "uptime", "disktest", "kill", "tasks",
    "ticks", "schedule", "pagetest", "proctest",
    "run"};

#define COMMAND_COUNT (sizeof(command_list) / sizeof(command_list[0]))

void autocomplete_command()
{
    int i;
    int matches = 0;
    int match_index = -1;

    if (input_cursor != input_pos)
        return;

    for (i = 0; i < input_pos; i++)
    {
        if (input[i] == ' ')
            return;
    }

    for (i = 0; i < COMMAND_COUNT; i++)
    {
        if (starts_with(command_list[i], input))
        {
            matches++;
            match_index = i;
        }
    }

    if (matches == 1)
    {
        int j = 0;

        while (command_list[match_index][j] && j < 62)
        {
            input[j] = command_list[match_index][j];
            j++;
        }

        input[j] = '\0';

        input_pos = j;
        input_cursor = j;

        redraw_input();
    }
    else if (matches > 1)
    {
        putchar('\n');

        for (i = 0; i < COMMAND_COUNT; i++)
        {
            if (starts_with(command_list[i], input))
            {
                print(command_list[i]);
                print("  ");
            }
        }

        putchar('\n');
        print("ValaOS> ");
        print(input);
        move_input_cursor(0);
    }
}

void keyboard_handler()
{
    if (!(inb(0x64) & 1))
        return;

    uint8_t scancode = inb(0x60);

    if (scancode == 0xE0)
    {
        extended_scancode = 1;
        return;
    }

    if (extended_scancode)
    {
        extended_scancode = 0;

        if (scancode & 0x80)
            return;

        uint8_t code = scancode & 0x7F;

        if (code == 75)
        {
            move_input_cursor(-1);
            return;
        }

        if (code == 77)
        {
            move_input_cursor(1);
            return;
        }

        if (code == 72)
        {
            if (history_count > 0 && history_index > 0)
            {
                history_index--;
                load_history_command(history_index);
            }

            return;
        }

        if (code == 80)
        {
            if (history_index < history_count - 1)
            {
                history_index++;
                load_history_command(history_index);
            }
            else if (history_index < history_count)
            {
                history_index = history_count;
                input_pos = 0;
                input_cursor = 0;
                input[0] = '\0';
                redraw_input();
            }

            return;
        }

        return;
    }

    uint8_t key_code = scancode & 0x7F;

    if (key_code == 42 || key_code == 54)
    {
        shift_pressed = !(scancode & 0x80);
        return;
    }

    if (scancode & 0x80)
    {
        key_down[key_code] = 0;
        return;
    }

    if (key_code == 58)
    {
        if (!key_down[key_code])
            caps_lock = !caps_lock;

        key_down[key_code] = 1;
        return;
    }

    if (key_code >= sizeof(key_map))
        return;

    if (key_down[key_code])
        return;

    key_down[key_code] = 1;

    if (key_code == 15)
    {
        autocomplete_command();
        hide_cursor();
        return;
    }

    char c = key_map[key_code];

    if (shift_pressed)
        c = shift_key_map[key_code];

    if (caps_lock && c >= 'a' && c <= 'z')
        c -= 'a' - 'A';
    else if (caps_lock && shift_pressed &&
             c >= 'A' && c <= 'Z')
        c += 'a' - 'A';

    if (c == '\b')
    {
        if (input_cursor > 0)
        {
            for (int i = input_cursor; i < input_pos; i++)
                input[i - 1] = input[i];

            input_pos--;
            input_cursor--;
            input[input_pos] = '\0';

            redraw_input();
        }
    }
    else if (c == '\n')
    {
        input[input_pos] = '\0';
        command_pending = 1;
    }
    else if (c && c != '\t' && input_pos < 63)
    {
        for (int i = input_pos; i > input_cursor; i--)
            input[i] = input[i - 1];

        input[input_cursor] = c;

        input_pos++;
        input_cursor++;
        input[input_pos] = '\0';

        redraw_input();
        history_index = history_count;
    }

    hide_cursor();
}

void keyboard_process_pending()
{
    if (!command_pending)
        return;

    command_pending = 0;

    save_history_command();
    history_index = history_count;

    execute_command();

    input_pos = 0;
    input_cursor = 0;
    input[0] = '\0';

    input_start_row = row;
    input_start_col = col;
    input_display_len = 0;
}

void keyboard_poll()
{
    keyboard_process_pending();
}
