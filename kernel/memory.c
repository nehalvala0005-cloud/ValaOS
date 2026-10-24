
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

    if (pages == 0 || pages > TOTAL_PAGES)
        return 0;

    if (allocated_pages > TOTAL_PAGES - pages)
        return 0;

    start = find_free_pages(pages);

    if (start < 0)
        return 0;

    slot = find_free_allocation_slot();

    if (slot < 0)
        return 0;

    for (i = 0; i < pages; i++)
        set_page_used((uint32_t)start + i);

    allocations[slot].start_page = (uint32_t)start;
    allocations[slot].page_count = pages;
    allocations[slot].used = 1;

    allocated_pages += pages;

    return &heap[(uint32_t)start * PAGE_SIZE];
}

static int free_pages(void *address, uint32_t pages,
                      int check_page_count)
{
    unsigned char *ptr;
    uint32_t offset;
    uint32_t page;
    uint32_t i;
    int slot;

    if (address == 0 || pages == 0 || pages > TOTAL_PAGES)
        return 0;

    ptr = (unsigned char *)address;

    if (ptr < &heap[0] || ptr >= &heap[TOTAL_MEMORY])
        return 0;

    offset = (uint32_t)(ptr - &heap[0]);

    if (offset % PAGE_SIZE != 0)
        return 0;

    page = offset / PAGE_SIZE;

    if (page >= TOTAL_PAGES || pages > TOTAL_PAGES - page)
        return 0;

    for (slot = 0; slot < MAX_ALLOCATIONS; slot++)
    {
        if (allocations[slot].used &&
            allocations[slot].start_page == page &&
            (!check_page_count ||
             allocations[slot].page_count == pages))
        {
            if (allocations[slot].page_count >
                allocated_pages)
                return 0;

            for (i = 0; i < allocations[slot].page_count; i++)
            {
                if (!page_is_used(page + i))
                    return 0;
            }

            pages = allocations[slot].page_count;

            for (i = 0; i < pages; i++)
                set_page_free(page + i);

            allocated_pages -= pages;

            allocations[slot].start_page = 0;
            allocations[slot].page_count = 0;
            allocations[slot].used = 0;

            return 1;
        }
    }

    return 0;
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

    if (size == 0)
        return 0;

    pages = size / PAGE_SIZE;

    if (size % PAGE_SIZE != 0)
        pages++;

    return allocate_pages(pages);
}

void kfree(void *address)
{
    unsigned char *ptr;
    uint32_t offset;
    uint32_t page;
    int slot;

    if (address == 0)
        return;

    ptr = (unsigned char *)address;

    if (ptr < &heap[0] || ptr >= &heap[TOTAL_MEMORY])
        return;

    offset = (uint32_t)(ptr - &heap[0]);

    if (offset % PAGE_SIZE != 0)
        return;

    page = offset / PAGE_SIZE;

    for (slot = 0; slot < MAX_ALLOCATIONS; slot++)
    {
        if (allocations[slot].used &&
            allocations[slot].start_page == page)
        {
            free_pages(address,
                       allocations[slot].page_count, 1);
            return;
        }
    }
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
