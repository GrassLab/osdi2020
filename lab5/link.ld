
SECTIONS
{
    . = 0xffff000000000000;
    . += 0x80000;
    _kernel_start = .;
    .text : { KEEP(*(.text.boot)) *(.text .text.* .gnu.linkonce.t*) }
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
    PROVIDE(_data = .);
    .data : { *(.data .data.* .gnu.linkonce.d*) }
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }
    .bootloader : {
        __bootloader_start = .;
        *(.bootloader);
        __bootloader_end = .;
    }

    . = ALIGN(4096);
    page_dir = .;
    .data.pgd : { . += ( 524 * (1 << 12)); } /*(1+1+2+512+8)*/
    . = ALIGN(4096);
    _kernel_end = .;
}
__bss_size = (__bss_end - __bss_start)>>3;
__kernel_size = (_kernel_end - _kernel_start)>>3;
