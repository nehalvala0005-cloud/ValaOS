extern void print(const char* str);

void syscall_handler(unsigned int syscall_number, unsigned int value) {
    char output[2];

    if (syscall_number == 1) {
        output[0] = (char)value;
        output[1] = '\0';

        print("\n[ USER ] sys_write: ");
        print(output);
        print("\n");
    }
}