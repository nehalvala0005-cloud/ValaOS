section .text

global keyboard_isr
extern keyboard_handler

keyboard_isr:
    pusha
    call keyboard_handler
    popa

    mov al, 0x20
    out 0x20, al

    iretd