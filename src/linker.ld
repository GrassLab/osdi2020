SECTIONS
{
  . = 0x80000;
  .text.boot : { *(.text.boot) }
  .text : { *(.text) }
  .data : { *(.data) }
   . = ALIGN(0x8);
  .bss : {
       bss_beg = . ;
       * (.bss);
       bss_end = . ;
   }
   /* __stack_top = ORIGIN(RAM) + LENGTH(RAM); */
}
