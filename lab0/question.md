# Lab0 Questions

> Q: What’s the RAM size of Raspberry Pi 3B+?
A: 1GB LPDDR2 SDRAM. Ref: https://en.wikipedia.org/wiki/Raspberry_Pi#Processor

---

> Q: What’s the cache size and level of Raspberry Pi 3B+?
A: 512 KB L2 cache. Ref: https://en.wikipedia.org/wiki/Raspberry_Pi#Processor

---

> Q: Explain each line of the above linker script.
```
SECTIONS
{
  . = 0x80000;
  .text : { *(.text) }
}
```
A:
```
/* Ref: https://wen00072.github.io/blog/2014/03/14/study-on-the-linker-script/ */
SECTIONS /* Describe the memory layout of the output file */
{
  . = 0x80000;
  /* 
   * '.', which is the location counter. If you do not specify the address of an output section in some other way,
   * the address is set from the current value of the location counter. The location counter is then incremented by
   * the size of the output section. At the start of the 'SECTIONS' command, the location counter has the value ‘0’.
   */
  .text : { *(.text) }
  /*
   * The '*' is a wildcard which matches any file name. The expression '*(.text)' means all '.text' input sections
   * in all input files. Put all the .text section machine code in all input object files to output file's '.text' section.
   *
}
```
