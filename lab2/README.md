# Bootloader
You practiced bare metal programming in Lab 1. Every time recompiling the kernel image, 
it’s mandatory to copy it to the SD card and place it into rpi3. This procedure is quite annoying.<br>
A more convenient way to load the recompiled kernel without touching the SD card could be achieved by UART.<br>
The program used to send kernel image to raspi in host can refer to [raspbootin](https://github.com/mrvn/raspbootin)<br>
<br>

## NOTE
The kernel image in SD card only has one job which is communicate with host loader server to get the new kernel 
and put on specific address, generally is 0x80000.<br><br>
The loader in raspi was loaded by VCore in 0x80000, so when you put the new kernel image in 0x80000, 
it may cover loader and cause error. So we have to shift the loader to 0x80000 - 1024.<br><br>
If you want to load to another address, you have to recompiler the new kernel by reset the starting address in linker script.
Because there are some instruction use absolute addressing, and it will cause error if you shift the loading address in memory.<br>

# Mailbox
It's a facilitate provided by BCM2835 Soc for communication between ARM and VCore.
refer to [raspberrypi wiki](https://github.com/raspberrypi/firmware/wiki/Mailboxes)<br>

## NOTE
The qemu can send the picture by vnc server. We can use mobaxterm to open vnc session.<br><br>
Mobaxterm VNC session setting config:<br>
In Advanced Vnc settings tab, click all the option. <br>
In network settings tab, click the "Connect through SSH gateway (jump host)".

# Framebuffer
Use mailbox to get the set up information of framebuffer, and try to show a picture on the screen.<br>
Each pixel use rgb format and express ad usigned int in buffer.

# PL011 UART
Mini UART’s clock is based on system clock. PL011 UART has it’s own seperate clock. 
You can get and set UART’s clock rate by mailbox. After that, you can set baud rate register according to UART’s base clock rate.
