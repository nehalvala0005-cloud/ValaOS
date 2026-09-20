section .multiboot
align 4
    dd 0x1BADB002
    dd 0
    dd -(0x1BADB002)

section .bss
align 16

stack_bottom:
    resb 16384
stack_top:

section .text

global start
global stack_top
extern kernel_main

start:
    cli

    lgdt [gdt_descriptor]

    mov ax, 0x08
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, stack_top

    jmp 0x10:protected_mode

protected_mode:
    call kernel_main

hang:
    cli
    hlt
    jmp hang

section .data

global gdt

gdt:
    dq 0x0000000000000000
    dq 0x00CF92000000FFFF
    dq 0x00CF9A000000FFFF
    dq 0x00CFFA000000FFFF
    dq 0x00CFF2000000FFFF
    dq 0x0000000000000000

gdt_descriptor:
    dw gdt_descriptor - gdt - 1
    dd gdt 