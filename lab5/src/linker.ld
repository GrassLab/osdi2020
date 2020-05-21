SECTIONS
{
    . = 0xffff000000000000; // kernel space
    . += 0x80000;
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
__bss_size = (__bss_end - __bss_begin)>>3;