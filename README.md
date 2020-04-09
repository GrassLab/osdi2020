# My OSDI 2020

## Author
| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`A071725`| `hank0438` | `陳廣翰` | hank0438@gmail.com |

## How to build

make 

## How to run

make run

## How to burn it for the first time?

dd if ./lab0/provided/nctuos.img /dev/sdb

## How to burn it into pi3

cp ./${LAB}/kernel8.img ${YOUR SD CARD} 

## How to screen it by UART?

* After setup the setting of your usb port to your vm
sudo screen /dev/ttyUSB0 115200 

## Directory structure

```shell=
.
├── include
│   ├── gpio.h
│   ├── homer.h
│   ├── info.h
│   ├── lfb.h
│   ├── loadimg.h
│   ├── mailbox.h
│   ├── power.h
│   ├── time.h
│   └── uart.h
├── LICENSE
├── Makefile
├── provided
│   └── nctuos.img
├── raspbootcom.py
├── raspbootin
│   ├── gpio.h
│   ├── homer.h
│   ├── kernel8.elf
│   ├── kernel8.img
│   ├── lfb.c
│   ├── lfb.h
│   ├── lfb.o
│   ├── link.ld
│   ├── main.c
│   ├── main.o
│   ├── Makefile
│   ├── mbox.c
│   ├── mbox.h
│   ├── mbox.o
│   ├── README.md
│   ├── start.o
│   ├── start.S
│   ├── uart.c
│   ├── uart.h
│   └── uart.o
├── README.md
├── src
│   ├── exception.c
│   ├── info.c
│   ├── lfb.c
│   ├── link.ld
│   ├── loadimg.c
│   ├── mailbox.c
│   ├── main.c
│   ├── power.c
│   ├── start.S
│   ├── time.c
│   └── uart.c
└── test
    ├── command.txt
    └── test.py

```

