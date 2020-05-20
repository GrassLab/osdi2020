
SECTIONS
{
     . = 0x80000;
     __kernel_start = .;
      .text.boot : {*(.text.boot) }
     .text : { *(.text) }
     .rodata : { *(.rodata) }
     
     . = ALIGN(0x00001000);
     PROVIDE(_data = .);
     .data : { *(.data) }
     
     .bss : {
         . = ALIGN(16);
         __bss_start = .;
         *(.bss*)
         __bss_end = .;
     }
          
     . = ALIGN(0x00001000);
     _end = .;
     _kernel_end = .; 
}
__bss_size = (__bss_end - __bss_start)>>3;
