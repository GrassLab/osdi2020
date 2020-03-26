SECTIONS
{
    . = 0x80000;
    start_begin = .;
    .text.boot : { *(.text.boot) }
    .text :  { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    . = ALIGN(0x8);
    bss_begin = .; /* Data that should be initialized to 0 */
    .bss : { *(.bss*) } 
    bss_end = .;
}