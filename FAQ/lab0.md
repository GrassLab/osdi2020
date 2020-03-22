# Lab 0: Environment Setup

## Requirement
- Cross-platform development

  ✅ Install the cross compiler on your host computer.
- QEMU

  ✅ Install qemu-system-aarch64 as an emulator for rpi3

  ✅ Build your first kernel image and check it by QEMU.
- Deploy to REAL rpi3

  ✅ Use either one of the methods to set up your sd card.

## Question
### What’s the RAM size of Raspberry Pi 3B+?
1GB
### What’s the cache size and level of Raspberry Pi 3B+?
 As below, it has 2 level cache.

 |   level  |  size  |
 | -------- | ------ |
 | L1 cache | 32kB   |
 | L2 cache | 512 KB |

### Explain each line of linker script.
```asm
SECTIONS
{
  . = 0x80000;
  .text : { *(.text) }
}
```
- line 1
    - `SECTIONS` is used to describe the layout of memory
- line3
    - set current location as 0x80000
- line 4
    - put code which matched `{ *(.text) }` into .text section