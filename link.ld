SECTIONS
{
    . = 0x80000;
    .text : { *( .text .text.* )  }
    .data : { *(.data .data.* ) }
    .bss : {
        _bss_start = .;
        *(.bss)
        _bss_end = .;
    }
    _end = .;

}
_bss_size = (_bss_end - _bss_start)>>3;
