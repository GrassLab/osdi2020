SECTIONS                    /* defines where the code is going to be stored in memory */
{
    . = 0x80000;            /* the kernel must be stored at 0x80000 in ram, because lower addresses are reserved */
    .text : { *(.text) }    /* maps the text segment from the assembly files */
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    . = ALIGN(0x8);
    bss_begin = .;
    .bss : { *(.bss*) }
    bss_end = .;
}
