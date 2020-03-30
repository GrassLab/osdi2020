SECTIONS
{
    . = _kbeg;
    .text : { KEEP(*(.text.boot)) *(.text .text.* .gnu.linkonce.t*) }
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
    PROVIDE(_data = .);
    .data : { 
        . = ALIGN(16);
        *(.data .data.* .gnu.linkonce.d*)
    }
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_beg = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }
    .bootloader : {
        . = ALIGN(4);
        _bootloader_beg = .;
        *(.bootloader*)
        _bootloader_end = .;
    }
    _kend = .;
   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
__bss_size = (__bss_end - __bss_beg)>>3;
__stack_top = _start;
