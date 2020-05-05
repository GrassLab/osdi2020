SECTIONS
{
     . = 0xffff000000000000;
     . += 0x80000;
     _kernel_start = .;
      
     .text.boot : {*(.text.boot) }
     .text : { *(.text) }
     .rodata : { *(.rodata) }
     .data : { *(.data) }
     .bss : {
         . = ALIGN(16);
         _bss_begin = .;
         *(.bss*)
         _bss_end = .;
     }
          
     . = ALIGN(0x00001000);
     pg_dir = .;
     .data.pgd : { . += ( 524* (1 << 12)); } /*(1+1+2+512+8)*/
     _kernel_end = .;
     
     . = ALIGN(0x00001000);
     _user_start = .;
     .text.user : { build/user* (.text) }
     .rodata.user : { build/user* (.rodata) }
     .data.user : { build/user* (.data) }
     .bss.user : { build/user* (.bss) }
     _user_end = .;
}
