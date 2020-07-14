SECTIONS
{
    . = 0x80000;
    start_begin = .;
    .text.boot : { *(.text.boot) }
    .text :  { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    . = ALIGN(0x8);
    __bss_begin = .;
    .bss : { *(.bss*) } 
    __bss_end = .;
}
