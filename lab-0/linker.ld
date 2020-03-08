SECTIONS
{
  . = 0x80000;         /* "." is location counter. This line means move location counter to 0x80000. */
  .text : { *(.text) } /* put .text section of all input files in the .text section. */
}
