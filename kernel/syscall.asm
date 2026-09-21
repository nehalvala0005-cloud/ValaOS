section .text

global syscall_isr
extern syscall_handler

syscall_isr:
    pusha
    call syscall_handler
    popa
    iretd