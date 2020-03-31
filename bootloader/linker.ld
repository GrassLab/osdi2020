SECTIONS
{
  . = 0x80000;
  .relocate :
  {
    KEEP(*(.text.relocate))
  }
  _begin = .;
  .text :
  {
    KEEP(*(.text.boot))
    *(.text)
  }
  .data :
  {
    *(.data)
  }
  .bss ALIGN(16) (NOLOAD) :
  {
    __bss_start = .;

    *(.bss)

    __bss_end = .;
  }
  _end = .;
}

__bss_size = (__bss_end - __bss_start) >> 3;