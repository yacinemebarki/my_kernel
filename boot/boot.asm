ORG 0x7C00
BITS 16

CODE_OFFSET equ 0x8
DATA_OFFSET equ 0x10

start:
    cli
    mov ax,0x00
    mov ds, ax
    mov es, ax
    mov sp, 0x7C00
    sti

load:
    cli
    lgdt[gdt_descriptor]
    mov eax,cr0
    or al, 1 
    mov cr0,eax
    jmp main


gdt_start:
    db 0x00000000
    
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b

    db 0x00

    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b

    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

BITS 32

main:
    mov ax, CODE_OFFSET
    mov ds,ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    mov gs, ax
    mov ebp, 0x900
    mov esp, ebp
    


TIMES 510-($-$$) db 0 
dw 0xAA55
