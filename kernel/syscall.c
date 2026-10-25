
/*
 * ValaOS
 * Copyright (c) 2026 Nehal Vala
 * All rights reserved.
 */

extern void print(const char* str);
extern void user_task_exit(unsigned int exit_code);

#define SYS_WRITE 1
#define SYS_EXIT 2

#define SYSCALL_OK 0
#define SYSCALL_EXIT 1
#define SYSCALL_ERROR -1

/* ABI: EAX is the syscall number and EBX is its scalar argument. SYS_WRITE
 * emits the low byte; SYS_EXIT records the full unsigned exit value. No
 * current syscall accepts or dereferences a user pointer. */
int syscall_handler(unsigned int syscall_number, unsigned int value) {
    char output[2];

    if (syscall_number == SYS_WRITE) {
        output[0] = (char)(value & 0xFFU);
        output[1] = '\0';

        print("\n[ USER ] sys_write: ");
        print(output);
        print("\n");

        return SYSCALL_OK;
    }

    if (syscall_number == SYS_EXIT) {
        user_task_exit(value);

        print("[ USER ] sys_exit called\n");
        print("[ OK ] User task terminated\n");

        return SYSCALL_EXIT;
    }

    return SYSCALL_ERROR;
}
