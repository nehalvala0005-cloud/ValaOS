section .text

global timer_isr
extern timer_handler

timer_isr:
    pusha
    call timer_handler
    popa
    iretd