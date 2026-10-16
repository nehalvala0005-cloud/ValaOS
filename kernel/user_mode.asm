section .text

global enter_user_mode
global user_return_esp
global user_return_eflags

user_return_esp:
    dd 0

user_return_eflags:
    dd 0

enter_user_mode:
    mov eax, esp
    mov [user_return_esp], eax

    pushfd
    pop eax
    mov [user_return_eflags], eax

    mov edx, [esp + 4]

    cli

    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push dword 0x23
    push edx

    pushfd
    or dword [esp], 0x200

    push dword 0x1B
    push dword 0x00400000

    iretd