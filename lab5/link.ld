SECTIONS
{
    . = 0xffff000000000000;
    . += 0x80000;
    _kernel_start = .;
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