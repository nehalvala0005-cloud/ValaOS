section .text

global syscall_isr
extern syscall_handler

syscall_isr:
    pusha

    push ebx
    push eax
    call syscall_handler
    add esp, 8

    popa
    iretd