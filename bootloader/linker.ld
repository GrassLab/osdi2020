SECTIONS
{
  . = 0x80000;
  .relocate :
  {
    KEEP(*(.text.relocate))
  }

  . = ALIGN(4096);
  _begin = .;
  .text :
  {
    KEEP(*(.text.boot))
    *(.text)
  }

  . = ALIGN(4096);
  .data :
  {
    *(.data)
  }

  . = ALIGN(4096);
  .bss (NOLOAD) :
  {
    __bss_start = .;

    *(.bss)

    __bss_end = .;
  }
  _end = .;
}

__bss_size = (__bss_end - __bss_start) >> 3;
__boot_loader = 0x3B3FC000;