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
  . = ALIGN(0x8);
  __bss_start__ = .;
  .bss :
  {
    *(.bss)
  }
  . = ALIGN(0x8);
  __bss_end__ = .;

  /* based on vc use 76 mb of ram, started from 0x3b400000*/
  . = 0x3b3ff000;
  .copyimg :
  {
    *(.copyimg);
  }
}

