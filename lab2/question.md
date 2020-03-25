# Lab2 Questions

> Q: In x86 machine, how the above 4 steps are implemented? Roughly describe it.
A: Load BIOS from ROM, perform hardware test. Select booting media. If disk is selected, MBR is read and then jump to OS initialization.

> Q: Calculate how long will it take for loading a 10MB kernel image by UART if baud rate is 115200.
A: Bit rate = Baud rate x the number of bit per baud. 115200 with 8N1 (8 bit data, 1 start 1 stop, no parity)2 bit per data byte overhead (start and stop bit). 115200 baud = 115200 * 0.8 / 8 = 11520 B/s. 10 * 1024 * 1024 = 910s

