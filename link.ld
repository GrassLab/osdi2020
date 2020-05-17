SECTIONS
{
    /*. = 0xffff000000000000; */
    /*. += 0x80000;*/
    . = 0x80000;

    _kernel_start = . ;

        .text   : { KEEP(*(.text.boot)) KEEP(*(.text.boot.mmu)) *(.text .text.* .gnu.linkonce.t*) }
        .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
        .data   : { *(.data) }
                
        . = ALIGN(16);
        __bss_start = .;
            .bss (NOLOAD) : {
                *(.bss .bss.*)
                *(COMMON)
                . = ALIGN(16);
            }
        __bss_end = .;      
        
        _PGD_START = 0x0;
        /* _PGD_START = .;*/ 
        
        .data.pgd : { . += (2 * (1 << 12)); }   /* where to store page table */

    _kernel_end = .;

    /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }

}
__bss_size = (__bss_end - __bss_start)>>3;