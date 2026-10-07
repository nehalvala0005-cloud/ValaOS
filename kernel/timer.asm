section .text

global timer_isr

extern timer_handler
extern is_user_task_running
extern task_switch_prepare
extern user_context_save

timer_isr:
    pusha

    call timer_handler

    call is_user_task_running
    cmp eax, 1
    je .user_task

    mov eax, esp
    push eax
    call task_switch_prepare
    add esp, 4

    mov esp, eax
    popa
    iretd

.user_task:
    mov eax, esp
    push eax
    call user_context_save
    add esp, 4

    popa
    iretd