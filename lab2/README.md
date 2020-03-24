# Lab 1 : Hello World+


## todo

## Mailbox

### Get the hardware’s information
- [ ] [required] Get the hardware’s information by mailbox and print them, you should at least print board revision and VC Core base address.

### PL011 UART
- [ ] [elective] Get or set UART clock by mailbox and replace mini UART by PL011 UART.

### Splash
- [ ] [elective] Set framebuffer by mailbox to show a splash image, show the result by qemu.

## Load kernel image

### Rpi3 boot flow
- [ ] [question] In x86 machine, how the above 4 steps are implemented? Roughly describe it.

### Load by UART
- [ ] [required] Implement bootloader can load kernel image by UART.
- [ ] [question] Calculate how long will it take for loading a 10MB kernel image by UART if baud rate is 115200.

### Load to certain address
- [ ] [elective] User can specify the kernel image’s loading address.