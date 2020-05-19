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

    . -= 0xffff000000000000;
    . = ALIGN(0x00001000);
    pg_dir = .;
    pgd_base = .;
    . += 0x1000;
    pud_base = .;
    . += 0x1000;
    pmd_base = .;
    . += 0x2000;
    pte_base = .;
    .data.pgd : { . += ( 524* (1 << 12)); } /*(1+1+2+512+8)*/
    . = ALIGN(0x00001000);
    . += 0xffff000000000000;
    _end = .;

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
__bss_size = (__bss_end - __bss_start)>>3;
