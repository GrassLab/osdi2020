SECTIONS
{
  . = 0xFFFF000000000000;
  . += 0x80000;
  __kernel_start = .;
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

  . = ALIGN(0x1000);
  pg_dir = .;
  .data.pgd :
  {
    . += (2 * (1 << 12));
  }
  . = ALIGN(0x1000);

  _end = .;
}

__bss_size = (__bss_end - __bss_start) >> 3;