section .text

global page_fault_isr
extern page_fault_handler

page_fault_isr:
    cli
    pusha

    mov eax, cr2
    push eax

    call page_fault_handler

    add esp, 4
    popa

    add esp, 4
    iretds