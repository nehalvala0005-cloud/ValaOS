section .text

global timer_isr

extern timer_handler
extern is_user_task_running
extern task_switch_prepare

timer_isr:
    pusha

    call timer_handler

    call is_user_task_running
    cmp eax, 1
    je .no_switch

    mov eax, esp
    push eax
    call task_switch_prepare
    add esp, 4

    mov esp, eax

.no_switch:
    popa
    iretd