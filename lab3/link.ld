SECTIONS
{
    . = 0x80000;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss  : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss);
        __bss_end = .;
    }
    _end = .;
}