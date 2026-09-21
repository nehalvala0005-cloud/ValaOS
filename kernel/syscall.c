extern void print(const char* str);

#define SYS_WRITE 1
#define SYS_EXIT  2

static int user_program_exited = 0;

void syscall_handler(unsigned int syscall_number, unsigned int value) {
    char output[2];

    if (syscall_number == SYS_WRITE) {
        output[0] = (char)value;
        output[1] = '\0';

        print("\n[ USER ] sys_write: ");
        print(output);
        print("\n");
    }

    else if (syscall_number == SYS_EXIT) {
        user_program_exited = 1;

        print("[ USER ] sys_exit called\n");
        print("[ OK ] User program terminated\n");

        while (1) {
            __asm__ volatile ("cli; hlt");
        }
    }
}

int has_user_program_exited() {
    return user_program_exited;
}