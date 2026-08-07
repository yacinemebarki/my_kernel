global irq0
extern irq0_handler

irq0:
    pusha

    push ds
    push es
    push fs
    push gs

    push esp
    call irq0_handler
    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds

    popa
    iretd