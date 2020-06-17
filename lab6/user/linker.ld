SECTIONS
{
    . = 0x0;
    /* the entry point of the user program */
    .text.entry : {*(text.entry) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : {*(.data)}
    . = ALIGN(8);
    .bss : { *(.bss*) }
}
