extern void print(const char* str);
extern void user_task_exit();

#define SYS_WRITE 1
#define SYS_EXIT 2

int syscall_handler(unsigned int syscall_number, unsigned int value) {
    char output[2];

    if (syscall_number == SYS_WRITE) {
        output[0] = (char)value;
        output[1] = '\0';

        print("\n[ USER ] sys_write: ");
        print(output);
        print("\n");

        return 0;
    }

    if (syscall_number == SYS_EXIT) {
        user_task_exit();

        print("[ USER ] sys_exit called\n");
        print("[ OK ] User task state: TERMINATED\n");

        return 1;
    }

    return 0;
}

void kernel_user_exit() {
    print("[ KERNEL ] User process exited\n");
    print("[ KERNEL ] Waiting for scheduler...\n");

    __asm__ volatile ("sti");

    while (1) {
        __asm__ volatile ("hlt");
    }
}