extern void keyboard_init();
extern void keyboard_poll();
extern void clear_screen();
extern void print(const char* str);
extern void paging_init();
extern int paging_enabled();
extern void page_fault_init();
extern void enable_write_protection();

void kernel_main() {
    __asm__ volatile ("cli");

    clear_screen();

    paging_init();
    page_fault_init();
    enable_write_protection();

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

    while (1) {
        keyboard_poll();
    }
}