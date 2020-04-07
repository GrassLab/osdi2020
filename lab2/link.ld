MEMORY {
    RAM(xrw): ORIGIN = 0x00060000, LENGTH = 8192K
}
SECTIONS
{
    . = 0x80000;
    .text : {*(.text.boot)} >RAM
    .data : { *(.data) } >RAM
    .bss : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss)
        __bss_end = .;
    } >RAM
    .bootloader : {
        __bootloader_start = .;
        *(.bootloader);
        __bootloader_end = .;
    } >RAM
    _end = .;
}
__bss_size = (__bss_end - __bss_start)>>3;
