ENTRY(_start)
OUTPUT_ARCH(aarch64)
SECTIONS
{
  . = 0xffff000000000000;
  . += 0x80000;
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
  .bss :
  {
    __bss_start__ = .;
    *(.bss)
    . = ALIGN(0x8);
    __bss_end__ = .;
  }
}

