typedef unsigned int uint32_t;

extern void print(const char *str);
extern int user_process_fault_terminate(uint32_t *pid);
extern void page_fault_recover(void) __attribute__((noreturn));

/* pusha frame followed by the CPU's page-fault frame. */
struct page_fault_frame
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t saved_esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t user_esp;
    uint32_t user_ss;
};

static volatile int page_fault_active;

static void print_hex(uint32_t value)
{
    static const char digits[] = "0123456789ABCDEF";
    char text[11];
    int i;

    text[0] = '0';
    text[1] = 'x';
    for (i = 0; i < 8; i++)
        text[2 + i] = digits[(value >> (28 - i * 4)) & 0xF];
    text[10] = '\0';
    print(text);
}

void page_fault_handler(uint32_t address, struct page_fault_frame *frame)
    __attribute__((noreturn));

void page_fault_handler(uint32_t address, struct page_fault_frame *frame)
{
    uint32_t pid = 0;

    /* A fault while printing or cleaning up must not recurse indefinitely. */
    if (page_fault_active)
    {
        __asm__ volatile("cli");
        for (;;)
            __asm__ volatile("hlt");
    }
    page_fault_active = 1;

    if ((frame->cs & 3U) == 3U)
    {
        print("\n[ FAULT ] User page fault: pid ");
        if (user_process_fault_terminate(&pid))
        {
            print_hex(pid);
            print(" address ");
            print_hex(address);
            print(" error ");
            print_hex(frame->error_code);
            print(" eip ");
            print_hex(frame->eip);
            print(" (ring 3); process terminated\n");
            page_fault_active = 0;
            page_fault_recover();
        }

        print("unknown process; address ");
        print_hex(address);
        print(" error ");
        print_hex(frame->error_code);
        print(" eip ");
        print_hex(frame->eip);
        print(" (ring 3)\n");
    }
    else
    {
        print("\n[ FATAL ] Kernel page fault: address ");
        print_hex(address);
        print(" error ");
        print_hex(frame->error_code);
        print(" eip ");
        print_hex(frame->eip);
        print(" cs ");
        print_hex(frame->cs);
        print(" (ring 0); halting\n");
    }

    __asm__ volatile("cli");
    for (;;)
        __asm__ volatile("hlt");
}
