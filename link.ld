SECTIONS
{
    . = 0xffff000000000000;
    . += 0x80000;

    _kernel_start = . ;

        .text.boot : { *(.text.boot) *(.text.boot.mmu)  }
        .text : { *(.text) }         
        .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
        .data   : { *(.data) }
                
        . = ALIGN(16);
        __bss_start = .;
            .bss : {
                *(.bss .bss.*)
                *(COMMON)
                . = ALIGN(16);
            }
        . = ALIGN(4096);
        __bss_end = .;      
        
        _PGD_START = .;
        
        .data.pgd : { . += (3 * (1 << 12)); }   /* where to store kernel page table */

    _kernel_end = .;

    /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }

}