typedef unsigned char uint8_t;

extern uint8_t inb(unsigned short port);

unsigned char* video = (unsigned char*)0xB8000;

char input[64];
int input_pos = 0;
int cursor = 0;

char key_map[] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,
    0,0,' '
};

void putchar(char c) {
    if (c == '\n') {
        cursor = ((cursor / 160) + 1) * 160;
        return;
    }

    video[cursor] = c;
    video[cursor + 1] = 0x07;
    cursor += 2;
}

void print(const char* str) {
    while (*str) {
        putchar(*str++);
    }
}

void clear_screen() {
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video[i] = ' ';
        video[i + 1] = 0x07;
    }

    cursor = 0;
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

void execute_command() {
    putchar('\n');

    if (compare(input, "help")) {
        print("Available commands:\n");
        print("help\n");
        print("info\n");
        print("clear\n");
        print("about\n");
    }
    else if (compare(input, "info")) {
        print("ValaOS v0.5\n");
        print("Architecture: x86\n");
        print("Keyboard: IRQ1\n");
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
    else if (input_pos > 0) {
        print("Unknown command\n");
    }

    input_pos = 0;
    input[0] = '\0';

    print("\nValaOS> ");
}

void keyboard_handler() {
    uint8_t scancode = inb(0x60);

    if (scancode & 0x80)
        return;

    if (scancode >= sizeof(key_map))
        return;

    char c = key_map[scancode];

    if (c == '\b') {
        if (input_pos > 0) {
            input_pos--;
            cursor -= 2;

            video[cursor] = ' ';
            video[cursor + 1] = 0x07;

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

void keyboard_init() {
    input_pos = 0;
    input[0] = '\0';
}