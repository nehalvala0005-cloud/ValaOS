typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

extern void page_fault_isr();
extern void print(const char* str);
extern void page_fault_handler(uint32_t address);

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr idt_descriptor;

static void outb(uint16_t port, uint8_t value) {
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static void idt_set_gate(int number, uint32_t handler) {
    idt[number].offset_low = handler & 0xFFFF;
    idt[number].selector = 0x08;
    idt[number].zero = 0;
    idt[number].flags = 0x8E;
    idt[number].offset_high = (handler >> 16) & 0xFFFF;
}

void page_fault_init() {
    int i;

    __asm__ volatile ("cli");

    for (i = 0; i < 256; i++) {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].flags = 0;
        idt[i].offset_high = 0;
    }

    idt_set_gate(14, (uint32_t)page_fault_isr);

    idt_descriptor.limit = sizeof(idt) - 1;
    idt_descriptor.base = (uint32_t)idt;

    __asm__ volatile (
        "lidt %0"
        :
        : "m"(idt_descriptor)
    );
}

void page_fault_handler(uint32_t address) {
    print("\n\n*** PAGE FAULT ***\n");
    print("Invalid memory access detected.\n");
    print("ValaOS protection system halted the CPU.\n");

    (void)address;

    while (1) {
        __asm__ volatile ("cli");
        __asm__ volatile ("hlt");
    }
}