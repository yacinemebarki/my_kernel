ORG 0x7C00
BITS 16

start:
    cli
    mov ax,0x00
    mov ds, ax
    mov es, ax
    mov sp, 0x7C00
    mov si, msg

print:
    lodsb
    cmp al,0
    je hang
    mov ah,0x0E
    int 0x10
    jmp print

hang:
    jmp hang

msg: db "welecom to my kernel",0

TIMES 510-($-$$) db 0 
dw 0xAA55
