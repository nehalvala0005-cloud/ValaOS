/*
 * ValaOS
 * Copyright (c) 2026 Nehal Vala
 * All rights reserved.
 */

 typedef unsigned int uint32_t;

extern void* page_alloc(uint32_t pages);
extern void page_free(void* address, uint32_t pages);
extern unsigned char __kernel_ro_start;
extern unsigned char __kernel_ro_end;

int set_page_flags(uint32_t *virtual_address, uint32_t flags);

#define PAGE_SIZE 4096
#define INITIAL_PAGE_TABLES 4
#define MAX_DYNAMIC_TABLES 12

#define PAGE_PRESENT 0x001
#define PAGE_WRITABLE 0x002
#define PAGE_USER 0x004

#define USER_CODE_VIRTUAL 0x00400000U
#define USER_STACK_VIRTUAL 0x00800000U

static uint32_t page_directory[1024]
    __attribute__((aligned(4096)));

static uint32_t page_tables[INITIAL_PAGE_TABLES][1024]
    __attribute__((aligned(4096)));

static uint32_t dynamic_tables[MAX_DYNAMIC_TABLES][1024]
    __attribute__((aligned(4096)));

static int dynamic_table_used[MAX_DYNAMIC_TABLES];

static void invalidate_page(uint32_t *virtual_address) {
    __asm__ volatile (
        "invlpg (%0)"
        :
        : "r"(virtual_address)
        : "memory"
    );
}

static uint32_t* allocate_dynamic_table() {
    uint32_t i;
    uint32_t j;

    for (i = 0; i < MAX_DYNAMIC_TABLES; i++) {
        if (!dynamic_table_used[i]) {
            dynamic_table_used[i] = 1;

            for (j = 0; j < 1024; j++)
                dynamic_tables[i][j] = 0;

            return dynamic_tables[i];
        }
    }

    return 0;
}

void paging_init() {
    uint32_t i;
    uint32_t j;
    uint32_t cr0;
    uint32_t address;
    uint32_t read_only_end;

    for (i = 0; i < 1024; i++)
        page_directory[i] = 0;

    for (i = 0; i < MAX_DYNAMIC_TABLES; i++)
        dynamic_table_used[i] = 0;

    for (i = 0; i < INITIAL_PAGE_TABLES; i++) {
        for (j = 0; j < 1024; j++) {
            page_tables[i][j] =
                (i * 1024 * PAGE_SIZE) +
                (j * PAGE_SIZE) |
                PAGE_PRESENT |
                PAGE_WRITABLE;
        }

        page_directory[i] =
            ((uint32_t)page_tables[i]) |
            PAGE_PRESENT |
            PAGE_WRITABLE;
    }

    __asm__ volatile (
        "mov %0, %%cr3"
        :
        : "r"(page_directory)
    );

    __asm__ volatile (
        "mov %%cr0, %0"
        : "=r"(cr0)
    );

    cr0 |= 0x80000000;

    __asm__ volatile (
        "mov %0, %%cr0"
        :
        : "r"(cr0)
    );

    /* Protect linked kernel text and read-only data from supervisor writes. */
    address = (uint32_t)&__kernel_ro_start & 0xFFFFF000;
    read_only_end = ((uint32_t)&__kernel_ro_end + PAGE_SIZE - 1) &
                    0xFFFFF000;
    while (address < read_only_end) {
        set_page_flags((uint32_t *)address, PAGE_PRESENT);
        address += PAGE_SIZE;
    }
}

int paging_enabled() {
    uint32_t cr0;

    __asm__ volatile (
        "mov %%cr0, %0"
        : "=r"(cr0)
    );

    return (cr0 & 0x80000000) != 0;
}

int map_page_flags(
    uint32_t *virtual_address,
    uint32_t *physical_address,
    uint32_t flags
) {
    uint32_t directory_index;
    uint32_t table_index;
    uint32_t* table;

    /* User mappings are owned by a process directory, never the kernel map. */
    if (flags & PAGE_USER)
        return 0;

    if ((uint32_t)virtual_address % PAGE_SIZE != 0)
        return 0;

    if ((uint32_t)physical_address % PAGE_SIZE != 0)
        return 0;

    directory_index = (uint32_t)virtual_address >> 22;
    table_index = ((uint32_t)virtual_address >> 12) & 0x3FF;

    if (directory_index >= 16)
        return 0;

    if (!(page_directory[directory_index] & PAGE_PRESENT)) {
        table = allocate_dynamic_table();

        if (!table)
            return 0;

        page_directory[directory_index] =
            ((uint32_t)table) |
            PAGE_PRESENT |
            PAGE_WRITABLE;
    }

    if (flags & PAGE_USER)
        page_directory[directory_index] |= PAGE_USER;

    table =
        (uint32_t*)(page_directory[directory_index] & 0xFFFFF000);

    flags &= 0xFFF;
    flags |= PAGE_PRESENT;

    table[table_index] =
        ((uint32_t)physical_address & 0xFFFFF000) |
        flags;

    invalidate_page(virtual_address);

    return 1;
}

int map_page(
    uint32_t *virtual_address,
    uint32_t *physical_address
) {
    return map_page_flags(
        virtual_address,
        physical_address,
        PAGE_PRESENT | PAGE_WRITABLE
    );
}

int unmap_page(uint32_t *virtual_address) {
    uint32_t directory_index;
    uint32_t table_index;
    uint32_t* table;

    if ((uint32_t)virtual_address % PAGE_SIZE != 0)
        return 0;

    directory_index = (uint32_t)virtual_address >> 22;
    table_index = ((uint32_t)virtual_address >> 12) & 0x3FF;

    if (directory_index >= 16)
        return 0;

    if (!(page_directory[directory_index] & PAGE_PRESENT))
        return 0;

    table =
        (uint32_t*)(page_directory[directory_index] & 0xFFFFF000);

    table[table_index] = 0;

    invalidate_page(virtual_address);

    return 1;
}

int is_page_mapped(uint32_t *virtual_address) {
    uint32_t directory_index;
    uint32_t table_index;
    uint32_t* table;

    if ((uint32_t)virtual_address % PAGE_SIZE != 0)
        return 0;

    directory_index = (uint32_t)virtual_address >> 22;
    table_index = ((uint32_t)virtual_address >> 12) & 0x3FF;

    if (directory_index >= 16)
        return 0;

    if (!(page_directory[directory_index] & PAGE_PRESENT))
        return 0;

    table =
        (uint32_t*)(page_directory[directory_index] & 0xFFFFF000);

    return (table[table_index] & PAGE_PRESENT) != 0;
}

int virtual_to_physical(
    uint32_t *virtual_address,
    uint32_t **physical_address
) {
    uint32_t directory_index;
    uint32_t table_index;
    uint32_t offset;
    uint32_t* table;
    uint32_t entry;

    directory_index = (uint32_t)virtual_address >> 22;
    table_index = ((uint32_t)virtual_address >> 12) & 0x3FF;
    offset = (uint32_t)virtual_address & 0xFFF;

    if (directory_index >= 16)
        return 0;

    if (!(page_directory[directory_index] & PAGE_PRESENT))
        return 0;

    table =
        (uint32_t*)(page_directory[directory_index] & 0xFFFFF000);

    entry = table[table_index];

    if (!(entry & PAGE_PRESENT))
        return 0;

    *physical_address =
        (uint32_t*)((entry & 0xFFFFF000) | offset);

    return 1;
}

uint32_t get_page_flags(uint32_t *virtual_address) {
    uint32_t directory_index;
    uint32_t table_index;
    uint32_t* table;

    directory_index = (uint32_t)virtual_address >> 22;
    table_index = ((uint32_t)virtual_address >> 12) & 0x3FF;

    if (directory_index >= 16)
        return 0;

    if (!(page_directory[directory_index] & PAGE_PRESENT))
        return 0;

    table =
        (uint32_t*)(page_directory[directory_index] & 0xFFFFF000);

    return table[table_index] & 0xFFF;
}

int set_page_flags(
    uint32_t *virtual_address,
    uint32_t flags
) {
    uint32_t directory_index;
    uint32_t table_index;
    uint32_t* table;

    /* Do not let callers elevate a shared kernel mapping to user access. */
    if (flags & PAGE_USER)
        return 0;

    if ((uint32_t)virtual_address % PAGE_SIZE != 0)
        return 0;

    directory_index = (uint32_t)virtual_address >> 22;
    table_index = ((uint32_t)virtual_address >> 12) & 0x3FF;

    if (directory_index >= 16)
        return 0;

    if (!(page_directory[directory_index] & PAGE_PRESENT))
        return 0;

    table =
        (uint32_t*)(page_directory[directory_index] & 0xFFFFF000);

    if (!(table[table_index] & PAGE_PRESENT))
        return 0;

    if (flags & PAGE_USER)
        page_directory[directory_index] |= PAGE_USER;

    table[table_index] =
        (table[table_index] & 0xFFFFF000) |
        (flags & 0xFFF) |
        PAGE_PRESENT;

    invalidate_page(virtual_address);

    return 1;
}

void enable_write_protection() {
    uint32_t cr0;

    __asm__ volatile (
        "mov %%cr0, %0"
        : "=r"(cr0)
    );

    cr0 |= 0x00010000;

    __asm__ volatile (
        "mov %0, %%cr0"
        :
        : "r"(cr0)
    );
}
uint32_t get_page_directory()
{
    uint32_t cr3;

    __asm__ volatile (
        "mov %%cr3, %0"
        : "=r"(cr3)
    );

    return cr3;
}
void switch_page_directory(uint32_t cr3)
{
    if (!cr3)
        return;

    __asm__ volatile (
        "mov %0, %%cr3"
        :
        : "r"(cr3)
        : "memory"
    );
}

uint32_t paging_get_kernel_directory()
{
    return (uint32_t)&page_directory[0];
}


uint32_t paging_create_process_directory(
    uint32_t code_page,
    uint32_t stack_page)
{
    uint32_t *directory;
    uint32_t *code_table;
    uint32_t *stack_table;
    uint32_t i;

    if (!code_page || !stack_page ||
        (code_page & (PAGE_SIZE - 1)) ||
        (stack_page & (PAGE_SIZE - 1)) ||
        code_page == stack_page ||
        !(page_directory[1] & PAGE_PRESENT) ||
        !(page_directory[2] & PAGE_PRESENT))
        return 0;

    directory = (uint32_t *)page_alloc(1);
    if (!directory)
        return 0;

    code_table = (uint32_t *)page_alloc(1);
    if (!code_table)
    {
        page_free(directory, 1);
        return 0;
    }

    stack_table = (uint32_t *)page_alloc(1);
    if (!stack_table)
    {
        page_free(code_table, 1);
        page_free(directory, 1);
        return 0;
    }

    for (i = 0; i < 1024; i++)
    {
        /* A kernel PDE must never inherit user permission into a process. */
        directory[i] = page_directory[i] & ~PAGE_USER;
        code_table[i] =
            ((uint32_t *)(page_directory[1] & 0xFFFFF000))[i] &
            ~PAGE_USER;
        stack_table[i] =
            ((uint32_t *)(page_directory[2] & 0xFFFFF000))[i] &
            ~PAGE_USER;
    }

    code_table[(USER_CODE_VIRTUAL >> 12) & 0x3FF] =
        (code_page & 0xFFFFF000) |
        PAGE_PRESENT |
        PAGE_USER;

    stack_table[(USER_STACK_VIRTUAL >> 12) & 0x3FF] =
        (stack_page & 0xFFFFF000) |
        PAGE_PRESENT |
        PAGE_WRITABLE |
        PAGE_USER;

    directory[USER_CODE_VIRTUAL >> 22] =
        ((uint32_t)code_table & 0xFFFFF000) |
        PAGE_PRESENT |
        PAGE_WRITABLE |
        PAGE_USER;

    directory[USER_STACK_VIRTUAL >> 22] =
        ((uint32_t)stack_table & 0xFFFFF000) |
        PAGE_PRESENT |
        PAGE_WRITABLE |
        PAGE_USER;

    return (uint32_t)directory;
}
