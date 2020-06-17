SECTIONS
{
    # . = 0xffff000000000000;
    # . += 0x80000;
    . = 0x80000;
    __kernel_start = .;
    .text.boot : { *(.text.boot) }
    .text :  { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    . = ALIGN(0x8);
    __bss_start = .;
    .bss : { *(.bss*) } 
    __bss_end = .;
}
__bss_size = (__bss_end - __bss_start)>>3;