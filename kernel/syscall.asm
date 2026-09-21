section .text

global syscall_isr
extern syscall_handler
extern kernel_user_exit

syscall_isr:
    pusha

    push ebx
    push eax
    call syscall_handler
    add esp, 8

    cmp eax, 1
    je syscall_exit

    popa
    iretd

syscall_exit:
    add esp, 32
    add esp, 20

    call kernel_user_exit

exit_hang:
    cli
    hlt
    jmp exit_hang