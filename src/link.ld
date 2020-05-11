SECTIONS
{
    . = 0xffff000000000000;
    . = ALIGN(0x00001000);
    . += 0x80000;
    __kernel_start = .;
    .text.boot : { *(.text.boot) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { 
        __bss_start = ALIGN(0x10); 
        *(.bss); 
        __bss_end = ALIGN(0x10); 
    }
    __kernel_end = .;
    pg_dir = .;
    .data.pgd : { . += (3 * (1 << 12)); }
}