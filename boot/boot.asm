ORG 0x7C00
BITS 16

boot:
    cli
    cld

    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    mov ah, 0x02      
    mov al, 30        
    mov ch, 0
    mov cl, 2         
    mov dh, 0

    int 0x13
    jc disk_error     

    jmp 0x1000:0x0000

disk_error:
    hlt
    jmp $

TIMES 510-($-$$) db 0
DW 0xAA55