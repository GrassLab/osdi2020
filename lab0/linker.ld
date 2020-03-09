SECTIONS  /* SECTIONS is used to describe the memory layout in runtime */
{
  . = 0x80000;           /* location counter := 0x80000  */
  .text : { *(.text) }   /* places the code "*(.text)" into the section .text */
  /* the *(.text) is config in source code (.S) */
}
