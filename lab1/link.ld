SECTIONS
{
    . = 0x80000;
    .text : {*(.text.boot)}
    .rodata : { *(.rodata) }
    PROVIDE(_data = .);
    .data : { *(.data) }
    .bss (NOLOAD) : {
        . = ALIGN(4096);
        __bss_start = .;
        *(.bss)
        *(COMMON)
        __bss_end = .;
    }
    _end = .;
}
__bss_size = (__bss_end - __bss_start)>>3;
