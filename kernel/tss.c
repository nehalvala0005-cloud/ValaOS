typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

struct tss_entry {
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed));

extern unsigned char stack_top;
extern uint64_t gdt[];

extern void tss_flush();

static struct tss_entry tss;

void tss_init() {
    uint32_t base;
    uint32_t limit;

    base = (uint32_t)&tss;
    limit = sizeof(tss) - 1;

    tss.esp0 = (uint32_t)&stack_top;
    tss.ss0 = 0x08;
    tss.iomap_base = sizeof(tss);

    gdt[5] =
        ((uint64_t)(limit & 0xFFFF)) |
        ((uint64_t)(base & 0xFFFFFF) << 16) |
        ((uint64_t)0x0089 << 40) |
        ((uint64_t)((limit >> 16) & 0x0F) << 48) |
        ((uint64_t)((base >> 24) & 0xFF) << 56);

    tss_flush();
}