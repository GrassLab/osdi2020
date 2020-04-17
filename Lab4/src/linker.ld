SECTIONS
{
    . = 0x80000;
    .text.boot : {*(.text.boot) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : {
        . = ALIGN(16);
        _bss_begin = .;
        *(.bss*)
        *(COMMON)
        _bss_end = .;
    }
    _end = .;
   
}
__bss_size = (_bss_end - _bss_begin)>>3;
