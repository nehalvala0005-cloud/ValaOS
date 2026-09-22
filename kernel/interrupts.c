typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

extern void timer_isr();
extern void scheduler_tick();
extern void syscall_isr();
extern void page_fault_isr();

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

static uint32_t timer_ticks = 0;

static void outb(uint16_t port, uint8_t value) {
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static void idt_set_gate(int number, uint32_t handler) {
    idt[number].offset_low = handler & 0xFFFF;
    idt[number].selector = 0x10;
    idt[number].zero = 0;
    idt[number].flags = 0x8E;
    idt[number].offset_high = (handler >> 16) & 0xFFFF;
}

static void idt_set_user_gate(int number, uint32_t handler) {
    idt[number].offset_low = handler & 0xFFFF;
    idt[number].selector = 0x10;
    idt[number].zero = 0;
    idt[number].flags = 0xEE;
    idt[number].offset_high = (handler >> 16) & 0xFFFF;
}
static void pic_init() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0xFE);
    outb(0xA1, 0xFF);
}

static void pit_init() {
    uint32_t divisor = 11931;

    outb(0x43, 0x36);

    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void interrupts_init() {
    int i;

    for (i = 0; i < 256; i++) {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].flags = 0;
        idt[i].offset_high = 0;
    }

    idt_set_gate(32, (uint32_t)timer_isr);
    idt_set_gate(14, (uint32_t)page_fault_isr);
    idt_set_user_gate(0x80, (uint32_t)syscall_isr);

    idt_descriptor.limit = sizeof(idt) - 1;
    idt_descriptor.base = (uint32_t)idt;

    __asm__ volatile (
        "lidt %0"
        :
        : "m"(idt_descriptor)
    );

    pic_init();
    pit_init();
}

void timer_handler() {
    timer_ticks++;

    outb(0x20, 0x20);
}

uint32_t get_timer_ticks() {
    return timer_ticks;
}