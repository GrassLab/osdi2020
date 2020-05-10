SECTIONS
{
  . = 0xffff000000000000;
  . += 0x80000;
  .text : { *(.text.boot) *(.text) }
  .data : { *(.data) }
  .bss : { _bstart = ALIGN(0x10) ; *(.bss) ; _bend = ALIGN(0x10) ; }
  .bootloader : {
	  __start_bootloader = . ;
	  *(.bootloader) ;
	  __stop_bootloader = . ;
  }
}
