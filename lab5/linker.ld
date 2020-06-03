SECTIONS
{
    . = 0xffff000000000000;
    kernel_start = 0x80000;
    . += kernel_start;

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
    . = ALIGN(0x1000);
    kernel_end = .;
}
