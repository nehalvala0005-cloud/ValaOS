extern void keyboard_init();
extern void keyboard_poll();
extern void clear_screen();
extern void print(const char* str);

void kernel_main() {
    clear_screen();

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