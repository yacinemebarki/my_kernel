BITS 32

extern _bss_start
extern _bss_end
global start

extern kernel

start:
    mov edi, _bss_start
    mov ecx, _bss_end
    sub ecx, edi
    xor eax, eax
    rep stosb
    call kernel
   
    
