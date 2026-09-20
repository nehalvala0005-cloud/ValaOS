section .text

global enter_user_mode

enter_user_mode:
    cli

    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push dword 0x23
    push dword 0x00801000

    pushfd

    push dword 0x1B
    push dword 0x00400000

    iretd