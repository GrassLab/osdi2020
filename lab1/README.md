# lab1 Hello World

## Basic initialization
1. Let only one core proceed, and let others enter a busy loop.
2. Initialize the BSS segment.
3. Set the stack pointer to an appropriate position.

### Note
1. You have to do the alignment in the bss segment of linker script. The alignment size is depend on the size you will initial in clear bss segment of startup.s. If you miss the alignment before __bss_end in linker script, the __bss_size will get the wrong value.

## Mini Uart
spec: BCM2835 / BCM2837 [datasheet](https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf)

### Note
1. In Windows OS, you have to set the baud rate in both **device manager** and your serial terminal(e.g. MobaXterm)
2. Do not use the power on uart, if you want to see the first message from raspi. After the uart-USB converter from raspi connect to PC, then power on the raspi.
3. **Check uart-USB converter**, you can connect the TX and RX on the converter (loopback). If you can type something on the serial terminal, then the converter is correct.

## Simple Shell
| command | description|
|---------|-----------|
| help | print all available commands |
| hello | print Hello World! |

Just a simple program.

## Timestamp
time = timer counter / timer frequency<br>
The information of timer can get from the ARM Register. 
[CNTFRQ_EL0](https://developer.arm.com/docs/ddi0595/c/aarch64-system-registers/cntfrq_el0), 
[CNTPCT_EL0](https://developer.arm.com/docs/ddi0595/b/aarch64-system-registers/cntpct_el0)

## Reboot
You have to know **watch dog timer**.<br>
The information of WDT in [BCM2835 register](https://elinux.org/BCM2835_registers#PM_WDOG)<br>
By setting the **watch dog timer** and **tick time**, then the raspi will reboot.

## Host Program
**\<lab1\>/host_serial/serial**<br>
There are some library for settimg uart in LINUX, you can implement a simple program to read a file and send the content to raspi3 by uart.<br>
Open two terminal, one to connect to raspi by uart, and the other execute the program. You will see some message send to the raspi3 on terminal one.
