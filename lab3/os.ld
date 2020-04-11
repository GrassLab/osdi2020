ENTRY(_start)
OUTPUT_ARCH(aarch64)
SECTIONS
{
  . = 0x80000;
  PROVIDE (__executable_start = .);
  .text :
  {
    *(.text)
  }
  PROVIDE (__etext = .);

  .data :
  {
    *(.data)
  }
  . = ALIGN(0x8);
  __bss_start__ = .;
  .bss :
  {
    *(.bss)
  }
  . = ALIGN(0x8);
  __bss_end__ = .;
}

