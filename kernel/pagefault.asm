section .text

global page_fault_isr
global page_fault_recover

extern page_fault_handler
extern user_return_esp
extern user_return_eflags
extern user_return_eip
extern user_return_ebx
extern user_return_esi
extern user_return_edi
extern user_return_ebp

page_fault_isr:
    ; The CPU has already pushed the page-fault error code. Keep it in the
    ; frame and pass both CR2 and the address of that frame to C.
    cli
    cld
    pusha

    mov eax, cr2
    mov edx, esp
    and esp, 0xFFFFFFF0        ; align the C call stack
    sub esp, 8                 ; reserve space for the two arguments
    push edx                    ; struct page_fault_frame *frame
    push eax                    ; fault address
    call page_fault_handler

.fatal_halt:
    cli
    hlt
    jmp .fatal_halt

; User-fault cleanup ran on the TSS kernel stack. Resume the kernel
; continuation saved by enter_user_mode, never the damaged user frame.
page_fault_recover:
    cli
    mov esp, [user_return_esp]
    add esp, 4                 ; consume enter_user_mode's saved return slot
    mov ecx, [user_return_eip]

    mov ax, 0x08                ; kernel data selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebx, [user_return_ebx]
    mov esi, [user_return_esi]
    mov edi, [user_return_edi]
    mov ebp, [user_return_ebp]

    push dword [user_return_eflags]
    popfd
    jmp ecx
