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

    push dword 0x202
    push dword 0x10
    push dword kernel_user_exit

    iretd

exit_hang:
    cli
    hlt
    jmp exit_hang