BITS 32

global restore_esp

restore_esp:
    mov eax, [esp + 4]
    mov esp, [eax + 16] 
    popa 
    iretd