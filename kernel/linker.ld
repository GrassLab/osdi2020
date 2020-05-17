SECTIONS
{
  . = 0xffff000000000000; /* Kernel space */
  . += 0x80000;

  __kernel_start = .;

  __text_start = .;
  .text : { *(.text.boot) *(.text) }
  __text_end = .;

  .rodata : { *(.rodata) }

  . = ALIGN(0x10);
  __bss_start = .;
  .bss : { *(.bss) }
  __bss_end = .;

  . = ALIGN(0x1000);
  page_table_start = .;
  . += ((1 + 1 + 2 + 1024) * 4096);

  __kernel_end = .;

  ENTRY(_start)
}
