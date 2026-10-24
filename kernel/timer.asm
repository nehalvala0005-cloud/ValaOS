section .text

global timer_isr

extern timer_handler
extern is_user_task_running
extern task_switch_prepare
extern user_context_save
extern user_context_restore
extern user_context_switch_prepare
extern user_schedule_next
extern get_next_user_context
extern user_process_reap_pending
extern user_return_esp
extern user_return_eflags

timer_isr:
    pusha
    cld

    call timer_handler

    ; The saved CS is at ESP+36 after PUSHA. Use the interrupted
    ; privilege level, not a mutable global process-state flag.
    test dword [esp + 36], 3
    jnz .user_task

.kernel_task:
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
    call user_context_switch_prepare
    add esp, 4

    ; A kill requested while user code is running is deferred until this
    ; interrupt gives us a safe kernel context in which to reclaim memory.
    call is_user_task_running
    cmp eax, 1
    je .resume_user

    call user_process_reap_pending

    ; Match the established SYS_EXIT return path: abandon the user IRQ
    ; frame and return to the kernel continuation saved by enter_user_mode.
    mov esp, [user_return_esp]
    mov ax, 0x08
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push dword [user_return_eflags]
    popfd
    ret

.resume_user:
    popa
    iretd
