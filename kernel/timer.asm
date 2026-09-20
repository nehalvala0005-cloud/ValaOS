section .text

global timer_isr
extern timer_handler
extern task_switch_prepare

timer_isr:
    pusha

    call timer_handler

    mov eax, esp
    push eax
    call task_switch_prepare
    add esp, 4

    mov esp, eax

    popa
    iretd