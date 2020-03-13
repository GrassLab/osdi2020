# Lab0: Environment Setup
## Introduction
The first step of creating a masterpiece is preparing the tool. You’re going to implement a 64-bit kernel on ARM CPU. Hence, you need a toolchain to help you finish the jobs.

In this lab, you’ll set up the working environment for future development.

## Goals of this lab
- Understand cross-platform development.
- Setup the working environment.
- Test your hardware.

## TODO list

### Cross compiler
- [x] [required]Install the cross compiler on your host computer.
- [x] [question]What’s the RAM size of Raspberry Pi 3B+?
  -  1GB RAM
- [x] [question]What’s the cache size and level of Raspberry Pi 3B+?
  - Raspberry Pi 3B+ use SoC [Broadcom BCM2837B0](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2837b0/README.md), has same architecture but higher frequency rate.
  - Have with 32kB Level 1 and 512kB Level 2 cache memory.

### Linker
- [x] [question]Explain each line of the above linker script.
```text
SECTIONS /* describe the memory layout */
{
  . = 0x80000; /* set the start addr */
  .text : { *(.text) } /* set all the *.text section put in this position */
}
```
### QEMU
- [x] [required]Install qemu-system-aarch64 as an emulator for rpi3.
 
### Check on QEMU
- [x] [required]Build your first kernel image and check it by QEMU.

### Flash bootable image to SD card
- [ ] [required]Use either one of the methods to set up your sd card.
