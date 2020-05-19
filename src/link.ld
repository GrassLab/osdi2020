SECTIONS
{
    . = 0xffff000000000000;
    . += 0x80000;
    user_begin = .;
	.text.user : { build/user* (.text) }
	.rodata.user : { build/user* (.rodata) }
	.data.user : { build/user* (.data) }
	.bss.user : { build/user* (.bss) }
	user_end = .;
    __kernel_start = .;
    .text.boot : { *(.text.boot) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    . = ALIGN(16);
    .data : { *(.data) }
    .bss : { 
        . = ALIGN(16);
        __bss_start = .; 
        *(.bss); 
        __bss_end = .; 
    }
    __kernel_end = .;
    . = ALIGN(0x00001000);
    pg_dir = .;
    .data.pgd : { . += ( 524* (1 << 12)); } /*(1+1+2+512+8)*/
    . = ALIGN(0x00001000);
}