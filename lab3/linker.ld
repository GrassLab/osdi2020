SECTIONS
{
    . = 0x80000;

    .text.boot : { *(.text.boot) }
    .text.section : { *(.text.section) }
    .text :  { *(.text) }
    .data : { *(.data) }
    . = ALIGN(0x8);
    bss_begin = .;
    .bss : { *(.bss*) }
    bss_end = .;
}
