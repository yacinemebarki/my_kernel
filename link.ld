ENTRY(kernel)

SECTIONS
{
    . = 0x10000;

    .text :
    {
        *(.text)
    }

    .data :
    {
        *(.data)
    }

    .bss : {
        _bss_start = .;
        *(.bss)
        *(COMMON)
        _bss_end = .;
    }
}