global isr13
extern fault_handler

isr13:
    push dword 13
    jmp isr_common

global isr14

isr14:
    push dword 14
    jmp isr_common

global isr8

isr8:
    push dword 8
    jmp isr_common

global isr0
isr0:
    push dword 0    
    push dword 0    
    jmp isr_common    

isr_common:
    pusha                  

    mov eax, [esp + 32]   
    mov ebx, [esp + 36]     
    mov ecx, [esp + 40]     

    push ecx
    push ebx
    push eax
    call fault_handler
    add esp, 12            

    popa
    add esp, 8               
    iretd

