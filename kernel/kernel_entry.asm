BITS 32
global start

extern kernel

start:
    mov ah,0x0E
    mov al,"H"
    int 0x10
    
hang:
    jmp hang