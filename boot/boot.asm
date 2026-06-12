BITS 16
ORG 0x7c00


main:
    HLT

hang:
    JMP hang

TIMES 510-($-$$) DB 0
DW 0xAA55

