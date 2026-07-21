global isr13
extern fault_handler

isr13:
    push dword 13
    jmp isr_common

global isr14

isr14:
    push dword 14
    jmp isr_common

global isr8

isr8:
    push dword 8
    jmp isr_common

isr_common:
    pusha
    call fault_handler

    add esp, 4
    popa
    add esp, 4
    iretd

