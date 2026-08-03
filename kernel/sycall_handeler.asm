global syscall_handler
extern syscall_dispatch   

syscall_handler:
    pusha             
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10        
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call syscall_dispatch   
    add esp, 4
    
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iret