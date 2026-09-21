extern void keyboard_init();
extern void keyboard_poll();
extern void clear_screen();
extern void print(const char* str);

extern void paging_init();
extern int paging_enabled();
extern void enable_write_protection();
extern void interrupts_init();
extern void task_context_init();
extern void tss_init();

extern int map_page_flags(
    unsigned int *virtual_address,
    unsigned int *physical_address,
    unsigned int flags
);

extern void enter_user_mode();

extern void user_task_start();

#define PAGE_PRESENT 0x001
#define PAGE_WRITABLE 0x002
#define PAGE_USER 0x004

static unsigned char user_code_page[4096]
    __attribute__((aligned(4096)));

static unsigned char user_stack_page[4096]
    __attribute__((aligned(4096)));

void user_mode_init() {
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

    map_page_flags(
        (unsigned int*)0x00400000,
        (unsigned int*)user_code_page,
        PAGE_PRESENT | PAGE_USER
    );

    map_page_flags(
        (unsigned int*)0x00800000,
        (unsigned int*)user_stack_page,
        PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER
    );
}

void kernel_main() {
    __asm__ volatile ("cli");

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

    print("ValaOS Kernel v1.0\n\n");
    print("[ OK ] Kernel loaded\n");
    print("[ OK ] VGA initialized\n");
    print("[ OK ] Keyboard driver initialized\n");
    print("[ OK ] Command shell initialized\n\n");

    print("Welcome to ValaOS!\n\n");

   print("ValaOS> ");

keyboard_init();

__asm__ volatile ("sti");

while (1) {
    keyboard_poll();
}
}