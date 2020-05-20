SECTIONS
{
    . = 0xffff000000000000;
    . += 0x80000;
    __kernel_start = .;
    .text.boot : { *(.text.boot) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    . = ALIGN(0x10);
    .data : { *(.data) }
    .bss : { 
        . = ALIGN(0x10);
        __bss_start = .; 
        *(.bss); 
        __bss_end = .; 
    }
    __kernel_end = .;
    . = ALIGN(0x1000);
    pg_dir = .;
    .data.pgd : { . += ( 524* (1 << 12)); } /*(1+1+2+512+8)*/
    . = ALIGN(0x1000);
}