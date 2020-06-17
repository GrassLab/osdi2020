SECTIONS
{
    . = 0x0;
    .text : {
        /* . = ALIGN(16); */
       KEEP(*(.text.entry)) *(.text .text.* .gnu.linkonce.t*)
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
   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
