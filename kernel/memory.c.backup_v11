typedef unsigned int uint32_t;

#define HEAP_SIZE 65536

static unsigned char heap[HEAP_SIZE];
static uint32_t heap_used = 0;
static uint32_t last_alloc_size = 0;

void* kmalloc(uint32_t size) {
    if (size == 0)
        return 0;

    if (heap_used + size > HEAP_SIZE)
        return 0;

    void* address = &heap[heap_used];

    heap_used += size;
    last_alloc_size = size;

    return address;
}

void kfree(void* address) {
    if (address == 0)
        return;

    if (heap_used == 0 || last_alloc_size == 0)
        return;

    if (address == &heap[heap_used - last_alloc_size]) {
        heap_used -= last_alloc_size;
        last_alloc_size = 0;
    }
}

uint32_t memory_used() {
    return heap_used;
}

uint32_t memory_free() {
    return HEAP_SIZE - heap_used;
}