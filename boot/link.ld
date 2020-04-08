SECTIONS                        /* defines where the code is going to be stored in memory */
{
    . = 0x80000;                /* the kernel must be stored at 0x80000 in ram, because lower addresses are reserved */
    .text : { *(.text) }        /* maps the text segment from the assembly files */
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : {
        __bss_start = .;
        *(.bss*)
        __bss_end = .;
    }
    .bootloader : {
        __bootloader_start = .;
        *(.bootloader)
        __bootloader_end = .;
    }
    _end = .;
}

__bss_size = (__bss_end - __bss_start) >> 3;
__bootloader_size = (__bootloader_end - __bootloader_start) >> 3;
