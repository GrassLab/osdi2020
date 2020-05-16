SECTIONS
{
    . = 0xffff000000000000;
    . += 0x80000;

    .text.boot : { *(.text.boot) }
    .text.mmu : { *(.text.mmu) }
    .text.section : { *(.text.exception) }
    .text.context : { *(.text.context) }
    .text :  { *(.text) }
    .data : { *(.data) *(.data.exception) }
    . = ALIGN(0x8);
    bss_begin = .;
    .bss : { *(.bss*) }
    bss_end = .;
}
