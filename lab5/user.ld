ENTRY(_start)
OUTPUT_ARCH(aarch64)
SECTIONS
{
  . = 0x0;
  .text.start :
  {
    *(.text.start)
  }
  .text :
  {
    *(.text)
  }
  .text :
  {
    *(.text)
  }
  . = ALIGN(0x8);
  .bss :
  {
    *(.bss)
    . = ALIGN(0x8);
  }
}
