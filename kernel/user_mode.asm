section .data

global user_return_esp
global user_return_eflags
global user_return_eip
global user_return_ebx
global user_return_esi
global user_return_edi
global user_return_ebp

user_return_esp:
    dd 0

user_return_eflags:
    dd 0

user_return_eip:
    dd 0

user_return_ebx:
    dd 0

user_return_esi:
    dd 0

user_return_edi:
    dd 0

user_return_ebp:
    dd 0

section .text

global enter_user_mode

enter_user_mode:
    mov eax, esp
    mov [user_return_esp], eax
    mov eax, [esp]
    mov [user_return_eip], eax
    mov [user_return_ebx], ebx
    mov [user_return_esi], esi
    mov [user_return_edi], edi
    mov [user_return_ebp], ebp

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
