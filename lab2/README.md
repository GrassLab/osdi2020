# Lab 1 : Hello World+


## todo

## Mailbox

### Get the hardware’s information
- [x] [required] Get the hardware’s information by mailbox and print them, you should at least print board revision and VC Core base address.

### PL011 UART
- [x] [elective] Get or set UART clock by mailbox and replace mini UART by PL011 UART.

### Splash
- [ ] [elective] Set framebuffer by mailbox to show a splash image, show the result by qemu.

## Load kernel image

### Rpi3 boot flow
- [x] [question] In x86 machine, how the above 4 steps are implemented? Roughly describe it.

1. GPU executes the first stage bootloader from ROM on the SoC.   
電源正常啟動後，x86 CPU 會先執行 0xFFFF0，也就是 BIOS ROM 的進入點。
由CPU啟動一个reset vector
POST(power-on self test)通常用 beep 聲來表示檢查結果。   
MBR 位於 hard disk 的 cylinder 0, head 0, sector 1，紀錄著 hard disk 的分割狀態，於開機時被載入至記憶體 0x0000:7C00。
GPT GPT是較MBR更先進的分割表格式，GPT使用了GUID分割表格這一更加符合現代需求的技術取代了老舊的MBR。  
啟動GRUB2   
如在 BIOS 所說，在做完 POST 之後，BIOS 會在儲存裝置找尋 boot sector，通常 boot sector 會位於 MBR (Master Boot Record)，BIOS 會把第一個找到的 boot record 載到 memory 中，然後開始執行
也因為 stage 1 boot loader 很小，所以它不聰明，認不得檔案系統，因此 stage 1 boot loader 的主要任務是載入 stage 1.5 boot loader

2. The first stage bootloader recognizes the FAT16/32 file system and loads the second stage bootloader bootcode.bin from SD card to L2 cache.   
GRUB2 stage 1.5 有足夠的空間，所以它包含一些常見檔案系統的 driver，像是 EXT, FAT, NTFS，這表示 GRUB2 stage 2 可以位於標準的 EXT 檔案系統內，明確地說，GRUB2 stage 2 的檔案位於 /boot/grub2。   
GRUB2 stage 1.5 的主要任務就是載入可以取得 GRUB2 stage 2 檔案的檔案系統 driver，及其它需要的 driver。   


3. bootcode.bin initializes SDRAM and loads start.elf   
BIOS 做POST就會找到 RAM 所以這邊就是 GRUB2 stage 2 直接去找firmware跟設定檔

4. start.elf initializes GPU’s firmware and reads config.txt, cmdline.txt and kernel8.img to start OS.   
Stage 2 boot loader 也稱為 kernel boot loader，它最主要的任務就是載入 Linux kernel 到 RAM 中，然後把控制權交給 kernel，   
在檔案系統中讀取設定檔 /boot/grub/grub.conf，產生開機選單，接著載入 Kernel + image file 到 memory，並把控制權交給 kernel。

<!-- 1. 開啟電源後, ARM CPU和SDRAM並未啟動, 先啟動的是GPU.
2. GPU首先啟動存在於主機板上的ROM中的第一階段開機程序來檢查SD卡中的檔案系統.
3. GPU載入SD卡中boot目錄中的第二階段開機程序bootcode.bin到L2 Cache中, 並執行bootcode.bin.
4. bootcode.bin啟動了SDRAM, 並載入第三階段開機程序loader.bin到RAM中, 並執行loader.bin.
5. loader.bin載入GPU的韌體start.elf
6. start.elf讀取設定檔config.txt及 cmdline.txt, 並載入最重要的Linux核心kernel.img.
7. start.elf在載入kernel.img後啟動了CPU.
8. 接下來就是Kernel的工作了, 也就是Raspbian開始在Raspberry Pi上運行了. -->

### Load by UART
- [x] [required] Implement bootloader can load kernel image by UART.   
Ok in qemu, but fail in raspi3   
- [x] [question] Calculate how long will it take for loading a 10MB kernel image by UART if baud rate is 115200.   
A: 7281.77s   
baud rate 115200 = 115200 bit/s = 14400 byte/s   
10 MB = 10 * 1024 * 1024 byte   
10 MB / 1440 byte = 10 * 1024 * 1024 / 1440 = 7281.777  

### Load to certain address
- [x] [elective] User can specify the kernel image’s loading address.
Only work on 0x80000   