SECTIONS
{
  . = 0x80000;
  .text : { *(.text.boot) *(.text) }
  .rodata : { *(.rodata) }
  . = ALIGN(0x10);
  __bss_start = .;
  .bss : { *(.bss) }
  __bss_end = .;

  ENTRY(_start)
}
