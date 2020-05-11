SECTIONS
{
    //. = 0xffff000000000000; 
    //. += 0x80000;
    . = 0x80000;
    __kernel_start = . ;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { 
        __bss_start = ALIGN(0x10); 
        *(.bss); 
        __bss_end = ALIGN(0x10); 
    }
    .bootloader : {
        __bootloader_start = . ;
        *(.bootloader);
        __bootloader_end = . ;
    }
    __kernel_end = . ;
    pg_dir = .;
}
__bss_size = (__bss_end - __bss_start)>>3;
__bootloader_size = (__bootloader_end - __bootloader_start)>>3;
