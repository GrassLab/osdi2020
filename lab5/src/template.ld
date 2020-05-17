SECTIONS
{
    . = 0xffff000000000000;
    . += 0x80000;
    _kbeg = .;
    .text : {
        /* . = ALIGN(16); */
        KEEP(*(.text.boot)) *(.text .text.* .gnu.linkonce.t*)
    }
    .rodata : {
        /* . = ALIGN(16); */
        *(.rodata .rodata.* .gnu.linkonce.r*)
    }
    PROVIDE(_data = .);
    .data : {
        /* . = ALIGN(16); */
        *(.data .data.* .gnu.linkonce.d*)
    }
    .bss (NOLOAD) : {
        /* . = ALIGN(16); */
        __bss_beg = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }
    .bootloader : {
        /* . = ALIGN(16); */
        _bootloader_beg = .;
        *(.bootloader*)
        _bootloader_end = .;
    }
    _kend = .;
    . = ALIGN(0x00001000);
    pg_dir = .;
   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
__bss_size = (__bss_end - __bss_beg)>>3;
__stack_top = _start;
