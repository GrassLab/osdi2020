SECTIONS
{
  . = 0xffff000000000000;
  . += 0x80000;
  _kernel_start = . ;
  .text : { *(.text.boot) *(.text) }
  .data : { *(.data) *(.rodata) }
  .bss : { _bstart = ALIGN(0x10) ; *(.bss) ; _bend = ALIGN(0x10) ; }
  .bootloader : {
	  __start_bootloader = . ;
	  *(.bootloader) ;
	  __stop_bootloader = . ;
  }
  _kernel_end = . ;
}
