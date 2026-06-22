global irq0
extern irq0_handler

irq0:
    pusha

    call irq0_handler
    popa

    iretd