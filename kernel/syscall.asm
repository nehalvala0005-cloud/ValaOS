section .text

global syscall_isr

extern syscall_handler
extern user_return_esp
extern user_return_eflags

syscall_isr:
    pusha

    push ebx
    push eax
    call syscall_handler
    add esp, 8

    cmp eax, 1
    je syscall_exit

    mov [esp + 28], eax
    popa
    iretd

syscall_exit:
    mov esp, [user_return_esp]
    push dword [user_return_eflags]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    pop eax
    push eax
    popfd
    ret