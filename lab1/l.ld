
SECTIONS
{
  . = 0x80000;         /* "." is location counter. This line means move location counter to 0x80000. */
  .text : 
  {
      KEEP(*(.text.boot)) 
      *(.text) 
  } /* put .text section of all input files in the .text section. */
  .rodata :{ *(.rodata) }
  .data : { *(.data) }
  .bss :
  {
      __bss_start = .;
	    *(.bss)
	    *(COMMON)
	    __bss_end = .;
  }
  _end = .;
}
__bss_size = (__bss_end - __bss_start)>>3;