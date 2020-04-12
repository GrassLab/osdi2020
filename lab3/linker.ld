SECTIONS
{
    . = 0x80000;

    .text.boot : { *(.text.boot) }
    .text.section : { *(.text.exception) }
    .text :  { *(.text) }
    .data : { *(.data) *(.data.exception) }
    . = ALIGN(0x8);
    bss_begin = .;
    .bss : { *(.bss*) }
    bss_end = .;
}
