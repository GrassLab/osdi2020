SECTIONS
{
    . = 0x800000;
    .text : 
    {
        KEEP(*(.text.boot)) 
        *(.text .text.* .gnu.linkonce.t*)
    }
    .rodata :
    {
        *(.rodata .rodata.* .gnu.linkonce.r*)
    }
    PROVIDE(_data = .);
    .data : 
    {
        *(.data .data.* .gnu.linkonce.d*)
    }
    .bss(NOLOAD) :
    {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }
    _end = .;
}
/* get the bss section size */
__bss_size = (__bss_end - __bss_start) >> 3;
