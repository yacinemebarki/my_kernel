BITS 32
global start

extern kernel

start:
    call kernel
    
hang:
    jmp hang