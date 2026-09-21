section .text

global syscall_isr

extern syscall_handler
extern user_return_esp

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

    sti

    mov esp, [user_return_esp]
    ret