ENTRY(start)

PHDRS
{
    text PT_LOAD FLAGS(5); /* read + execute */
    data PT_LOAD FLAGS(6); /* read + write */
}

SECTIONS
{
    . = 1M;

    .text ALIGN(4K) : {
        __kernel_ro_start = .;
        *(.multiboot)
        *(.text)
    } :text

    .rodata ALIGN(4K) : {
        *(.rodata)
        *(.eh_frame)
    } :text

    __kernel_ro_end = .;

    .data ALIGN(4K) : {
        *(.data)
    } :data

    .bss ALIGN(4K) : {
        *(.bss)
        *(COMMON)
    } :data
}
