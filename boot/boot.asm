ORG 0x7C00
BITS 16

CODE_OFFSET equ 0x08
DATA_OFFSET equ 0x10

mmap_ent equ 0x500

jmp start
boot_drive db 0


do_e820:
    mov di, 0x504
    xor ebx, ebx
    xor bp, bp
    mov edx, 0x0534D4150
    mov eax, 0xE820
    mov [es:di+20], dword 1
    mov ecx, 24
    int 0x015
    jc short failed
    mov edx, 0x0534D4150
    cmp eax, edx
    test ebx,ebx
    je short failed
    jmp short loopp

search:
    mov edx,0x534D4150
    mov eax, 0xE820
    mov [es:di + 20], dword 1
    mov ecx,24
    int 0x015
    jc failed
    cmp eax, 0x534D4150
    jne failed

loopp:
    jcxz skip
    cmp ecx,20
    jbe short next
    test byte [es:di + 20], 1	
	je short end

next:
    mov ecx, [es:di + 8]
    or ecx, [es:di + 12]
    jz skip
    inc bp
    add di, 24

skip:
    test ebx, ebx
    jne search

end:
    mov [es:mmap_ent], bp
    clc
    ret

failed:
    stc
    ret

start:
    cli
    mov [boot_drive], dl

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    call do_e820
    jc disk_error

    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    mov ah, 0x02      
    mov al, 8
    mov ch, 0         
    mov cl, 2        
    mov dh, 0         
    mov dl, [boot_drive]

    int 0x13
    jc disk_error

    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp CODE_OFFSET:protected_mode

disk_error:
    mov ah,0x0E
    mov al,'E'
    int 10h

    jmp $


gdt_start:

gdt_null:
    dq 0

gdt_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00

gdt_data:
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

protected_mode:

    mov ax, DATA_OFFSET
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000
    mov word [0xb8000],0x0F5
    jmp 0x10000

hang:
    jmp hang

TIMES 510-($-$$) db 0
dw 0xAA55

