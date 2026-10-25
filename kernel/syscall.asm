section .text

global syscall_isr

extern syscall_handler
extern user_return_esp
extern user_return_eflags
extern user_return_ebx
extern user_return_esi
extern user_return_edi
extern user_return_ebp

syscall_isr:
    ; A user can enter with DF set. Keep the saved EFLAGS in the CPU frame,
    ; but give kernel C code the ABI-required clear direction flag.
    cld
    pusha

    ; Preserve the PUSHA frame while aligning the temporary C call stack.
    ; Align ESP before CALL; the pushed return address then gives the C
    ; callee the i386 GCC-required entry alignment.
    mov esi, esp
    and esp, 0xFFFFFFF0
    sub esp, 8
    push ebx
    push eax
    call syscall_handler
    add esp, 8

    cmp eax, 1
    je syscall_exit

    mov [esi + 28], eax
    mov esp, esi
    popa
    iretd

syscall_exit:
    mov esp, [user_return_esp]
    push dword [user_return_eflags]

    mov ax, 0x08
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; The user frame is abandoned. Restore the kernel caller's callee-saved
    ; registers before resuming its continuation.
    mov ebx, [user_return_ebx]
    mov esi, [user_return_esi]
    mov edi, [user_return_edi]
    mov ebp, [user_return_ebp]

    pop eax
    push eax
    popfd
    ret
