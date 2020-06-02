SECTIONS
{
     . = 0xffff000000000000;
     . += 0x80000;
     __kernel_start = .;
      .text.boot : {*(.text.boot) }
     .text : { *(.text) }
     .rodata : { *(.rodata) }
     
     . = ALIGN(16);
     .data : { *(.data) }
     
     .bss : {
         . = ALIGN(16);
         __bss_start = .;
         *(.bss*)
         __bss_end = .;
     }
          
     . = ALIGN(0x00001000);
     pg_dir = .;
     .data.pgd : { . += ( 524* (1 << 12)); } /*(1+1+2+512+8)*/
     . = ALIGN(0x00001000);
     __kernel_end = .; 
}