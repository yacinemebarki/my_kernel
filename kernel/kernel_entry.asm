BITS 32
global start

extern kernel

start:
    call kernel
    jmp hang

hang:
    jmp hang
    
