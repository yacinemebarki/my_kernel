BITS 32

global load_page_directory
global enable_paging
global page_directory
global page_table

section .bss
align 4096
page_table: resd 1024

align 4096
page_directory: resd 1024

section .text

load_page_directory:
    mov eax, [esp + 4]
    mov cr3, eax
    ret

enable_paging:
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ret