ENTRY(_start)
OUTPUT_ARCH(aarch64)
SECTIONS
{
  . = 0x80000;
  .text :
  {
    *(.text)
  }
  .data :
  {
    *(.data)
  }
  __bss_start__ = .;
  .bss :
  {
    *(.bss)
  }
  __bss_end__ = .;
}

