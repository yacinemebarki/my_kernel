global enter_user_mode

enter_user_mode:
    
    mov eax, [esp + 4]
    mov ebx, [esp + 8]

    mov dx, 0x23
    mov ds, dx
    mov es, dx
    mov fs, dx
    mov gs, dx

    push 0x23
    push ebx
    pushfd

    pop ecx
    or ecx, 0x200
    push ecx

    push 0x1B

    push eax

    iret
