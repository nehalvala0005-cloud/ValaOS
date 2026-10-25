
/*
 * ValaOS
 * Copyright (c) 2026 Nehal Vala
 * All rights reserved.
 */

typedef unsigned int uint32_t;

#define PAGE_SIZE 4096U
#define TOTAL_MEMORY (4U * 1024U * 1024U)
#define TOTAL_PAGES (TOTAL_MEMORY / PAGE_SIZE)
#define MAX_ALLOCATIONS 128

struct allocation
{
    uint32_t start_page;
    uint32_t page_count;
    int used;
};

static unsigned char heap[TOTAL_MEMORY]
    __attribute__((aligned(PAGE_SIZE)));

static unsigned char page_bitmap[TOTAL_PAGES / 8];

static struct allocation allocations[MAX_ALLOCATIONS];

static uint32_t allocated_pages = 0;

static uint32_t allocator_lock(void)
{
    uint32_t flags;

    __asm__ volatile("pushfl; popl %0; cli" : "=r"(flags) : : "memory");
    return flags;
}

static void allocator_unlock(uint32_t flags)
{
    __asm__ volatile("pushl %0; popfl" : : "r"(flags) : "memory", "cc");
}

static int heap_address_to_page(void *address, uint32_t *page)
{
    uint32_t heap_start = (uint32_t)&heap[0];
    uint32_t value = (uint32_t)address;
    uint32_t offset;

    if (!address || !page || value < heap_start)
        return 0;

    offset = value - heap_start;

    if (offset >= TOTAL_MEMORY || offset % PAGE_SIZE != 0)
        return 0;

    *page = offset / PAGE_SIZE;
    return 1;
}

static void set_page_used(uint32_t page)
{
    page_bitmap[page / 8] |=
        (unsigned char)(1U << (page % 8));
}

static void set_page_free(uint32_t page)
{
    page_bitmap[page / 8] &=
        (unsigned char)~(1U << (page % 8));
}

static int page_is_used(uint32_t page)
{
    return (page_bitmap[page / 8] &
            (1U << (page % 8))) != 0;
}

static int find_free_pages(uint32_t pages)
{
    uint32_t i;
    uint32_t count = 0;
    uint32_t start = 0;

    if (pages == 0 || pages > TOTAL_PAGES)
        return -1;

    for (i = 0; i < TOTAL_PAGES; i++)
    {
        if (!page_is_used(i))
        {
            if (count == 0)
                start = i;

            count++;

            if (count == pages)
                return (int)start;
        }
        else
        {
            count = 0;
        }
    }

    return -1;
}

static int find_free_allocation_slot(void)
{
    int i;

    for (i = 0; i < MAX_ALLOCATIONS; i++)
    {
        if (!allocations[i].used)
            return i;
    }

    return -1;
}

static void *allocate_pages(uint32_t pages)
{
    int start;
    int slot;
    uint32_t i;
    uint32_t saved_flags;
    void *result = 0;

    saved_flags = allocator_lock();

    if (pages == 0 || pages > TOTAL_PAGES)
        goto done;

    if (allocated_pages > TOTAL_PAGES - pages)
        goto done;

    start = find_free_pages(pages);

    if (start < 0)
        goto done;

    slot = find_free_allocation_slot();

    if (slot < 0)
        goto done;

    for (i = 0; i < pages; i++)
        set_page_used((uint32_t)start + i);

    allocations[slot].start_page = (uint32_t)start;
    allocations[slot].page_count = pages;
    allocations[slot].used = 1;

    allocated_pages += pages;

    result = &heap[(uint32_t)start * PAGE_SIZE];

done:
    allocator_unlock(saved_flags);
    return result;
}

static int free_pages(void *address, uint32_t pages,
                      int check_page_count)
{
    uint32_t page;
    uint32_t i;
    int slot;
    int result = 0;
    uint32_t saved_flags;

    saved_flags = allocator_lock();

    if (pages == 0 || pages > TOTAL_PAGES ||
        !heap_address_to_page(address, &page) ||
        page >= TOTAL_PAGES || pages > TOTAL_PAGES - page)
        goto done;

    for (slot = 0; slot < MAX_ALLOCATIONS; slot++)
    {
        if (allocations[slot].used &&
            allocations[slot].start_page == page &&
            (!check_page_count ||
             allocations[slot].page_count == pages))
        {
            if (allocations[slot].page_count >
                allocated_pages)
                goto done;

            if (allocations[slot].page_count == 0 ||
                allocations[slot].page_count > TOTAL_PAGES - page)
                goto done;

            for (i = 0; i < allocations[slot].page_count; i++)
            {
                if (!page_is_used(page + i))
                    goto done;
            }

            pages = allocations[slot].page_count;

            for (i = 0; i < pages; i++)
                set_page_free(page + i);

            allocated_pages -= pages;

            allocations[slot].start_page = 0;
            allocations[slot].page_count = 0;
            allocations[slot].used = 0;

            result = 1;
            goto done;
        }
    }

done:
    allocator_unlock(saved_flags);
    return result;
}

void *page_alloc(uint32_t pages)
{
    return allocate_pages(pages);
}

void page_free(void *address, uint32_t pages)
{
    free_pages(address, pages, 1);
}

void *kmalloc(uint32_t size)
{
    uint32_t pages;

    if (size == 0 || size > TOTAL_MEMORY)
        return 0;

    /* Subtract before rounding so the arithmetic cannot wrap at UINT32_MAX. */
    pages = 1 + ((size - 1) / PAGE_SIZE);

    return allocate_pages(pages);
}

void kfree(void *address)
{
    uint32_t page;
    uint32_t saved_flags;
    int slot;

    if (address == 0)
        return;

    saved_flags = allocator_lock();

    if (!heap_address_to_page(address, &page))
    {
        allocator_unlock(saved_flags);
        return;
    }

    for (slot = 0; slot < MAX_ALLOCATIONS; slot++)
    {
        if (allocations[slot].used &&
            allocations[slot].start_page == page)
        {
            free_pages(address,
                       allocations[slot].page_count, 1);
            allocator_unlock(saved_flags);
            return;
        }
    }

    allocator_unlock(saved_flags);
}

uint32_t memory_used(void)
{
    return allocated_pages * PAGE_SIZE;
}

uint32_t memory_free(void)
{
    return TOTAL_MEMORY - memory_used();
}

uint32_t memory_total(void)
{
    return TOTAL_MEMORY;
}

uint32_t memory_page_size(void)
{
    return PAGE_SIZE;
}

uint32_t memory_total_pages(void)
{
    return TOTAL_PAGES;
}

uint32_t memory_used_pages(void)
{
    return allocated_pages;
}

uint32_t page_allocator_page_size(void)
{
    return PAGE_SIZE;
}
