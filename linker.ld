SECTIONS
{
  . = 0xffff000000000000; /* Kernel space */
  . += 0x80000;
  __text_start = .;
  .text : { *(.text.boot) *(.text) }
  __text_end = .;

  .rodata : { *(.rodata) }

  . = ALIGN(0x10);
  __bss_start = .;
  .bss : { *(.bss) }
  __bss_end = .;

  ENTRY(_start)
}
