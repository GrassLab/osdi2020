Lab2
===
#### `R1` Get the hardware’s information by mailbox and print them, you should at least print board revision and VC Core base address.
With hardware platform [Raspberry Pi 3 Model B+](https://www.raspberrypi.org/products/raspberry-pi-3-model-b-plus/), you can get these info:
```
VC Core base addr: 0x3B400000
VC memory size: 0x4C00000
Board revision: 0xA020D3
```

#### `E1` Get or set UART clock by mailbox and replace mini UART by PL011 UART.

#### `E2` Set framebuffer by mailbox to show a splash image, show the result by qemu.
※ homer.h is a picture which been converted (from .jpg .png) to .h type.

#### `Q1` In x86 machine, how the above 4 steps are implemented? Roughly describe it.
1. 開機後會產生一組 reset 訊號
2. CPU 會到 0xFFFF_FFF0h 去 fetch 第一條指令，由此跳到 ROM 裡面 BIOS 所在位置
3. 接下來 BIOS 會做 Power on Self Test (POST)，且到 CMOS 去讀取 configuration
4. BIOS 在指定設備中尋找 MBR，並將 Booloader（存放在 MBR 的前 446 bytes）載入 Main Memory (RAM)
5. First Stage Bootloader 負責讀進 Second Stage BootLoader
6. Second Stage BootLoader 會讀取 configuration file (Ex. grub.conf) 去啟動 boot selection menu，最後將 kernel image 載入 Memory 中，並把控制權交給 kernel
※ 通常把 First Stage Bootloader 和 Second Stage BootLoader 合稱為 Linux Loader (LILO) 或 GRand Unified Bootloader (GRUB)

#### `R2` Implement bootloader can load kernel image by UART.
Under Windows machine, I download the Java version kernel sender [Raspbootin64Client](https://github.com/milanvidakovic/Raspbootin64Client).
- Usage
```
java -jar Raspbootin64Client.jar <com_port> <file_path>
```
- Example
```
java -jar Raspbootin64Client.jar COM3 C:\Users\User\Desktop\OSDI\kernel8.img
```

#### `Q2` Calculate how long will it take for loading a 10MB kernel image by UART if baud rate is 115200.
baud rate = 115200 bit/s = 14400 bytes/s 
10 MB = 10 \* 2<sup>20</sup> = 10485760‬ bytes
10485760‬ \ 14400 ≒ 728.178 s

#### `E3` User can specify the kernel image’s loading address.