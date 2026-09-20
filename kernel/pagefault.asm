section .text

global page_fault_isr
extern page_fault_handler

page_fault_isr:
    cli
    call page_fault_handler

halt:
    cli
    hlt
    jmp halt