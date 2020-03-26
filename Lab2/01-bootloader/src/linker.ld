SECTIONS
{
    . = 0x80000 - 2048;
    PROVIDE(_code =.);
    
    .text.boot : { *(.text.boot) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    PROVIDE(_data = .);
    .data : { *(.data) }
    
    .bss (NOLOAD) : {
        . = ALIGN(16);
        _bss_begin = .;
        *(.bss*)
        *(COMMON)
        _bss_end = .;
    }
    _end = .;

}

__bss_size = (_bss_end - _bss_begin)>>3; /* >>3: bit to byte */
__loader_size = (_end - _code )>>3;    
