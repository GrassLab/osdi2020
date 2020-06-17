SECTIONS
{
    . = 0x80000;
    
    .text : { *(.text) }

    .data : { *(.data) }

    .rodata : { *(.rodata) }

    . = ALIGN(8);
    __bss_start = .;

    .bss  : { *(.bss ) }

    __bss_end = .;
}
