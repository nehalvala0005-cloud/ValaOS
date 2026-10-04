/*
 * ValaOS
 * Copyright (c) 2026 Nehal Vala
 * All rights reserved.
 */

 typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

struct task {
    uint32_t pid;
    const char* name;
    const char* state;
    uint32_t stack_base;
    uint32_t stack_top;
    struct {
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


extern void* kmalloc(unsigned int size);
extern void kfree(void* address);
extern unsigned int memory_used();
extern unsigned int memory_free();
extern void enter_user_mode();
extern void user_task_start();

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
extern void vfs_list_programs();
extern int vfs_read_file(const char* name, char* buffer, int max_size);
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


extern void execute_command();

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
