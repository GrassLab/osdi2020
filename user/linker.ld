ENTRY(_user_start)

SECTIONS
{
  .text : { *(.text.boot) *(.text) }

  .rodata : { *(.rodata) }
}
