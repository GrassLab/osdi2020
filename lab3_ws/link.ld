
SECTIONS
{
    . = 0x80000;
    .text : {KEEP(*(.text.boot)) *(.text .text.*)}
    . = ALIGN(4096);
    .data : { *(.data) } 
    .bss : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss)
        __bss_end = .;
    } 
    .bootloader : {
        __bootloader_start = .;
        *(.bootloader);
        __bootloader_end = .;
    } 
    _end = .;
}
__bss_size = (__bss_end - __bss_start)>>3;
