extern void print(const char* str);

void syscall_handler() {
    print("\n[ OK ] Ring 3 -> Ring 0 syscall\n");
}