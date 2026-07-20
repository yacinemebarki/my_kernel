global irq0
extern irq0_handler

irq0:
    pusha
    push esp
    call irq0_handler
    add esp, 4
    popa

    iretd