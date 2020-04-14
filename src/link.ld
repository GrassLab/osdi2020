SECTIONS
{
    . = 0x80000;
    PROVIDE(__loader_start = .);
    .text : { KEEP(*(.text.boot)) *(.text .text.*) }
    .data : { *(.data .data.*) }
    .bss : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        __bss_end = .;
    }
    __loader_end = .;

}
__bss_size = (__bss_end - __bss_start)>>3;
__loader_size = (__loader_end - __loader_start)>>3;
