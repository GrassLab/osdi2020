# My OSDI 2020

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`0856168`| `ex7763` | `許博鈞` | hpc.cs08g@nctu.edu.tw |

## Question

### Lab 0
Q:
What's the RAM size of Raspberry Pi 3B+?
A:
1GB SRAM

Q:
What's the cache size and level of Raspberry Pi 3B+?
A:
CPU: ARM Cortex-A53
Cache:
    L1 Cache: 32kB
    L2 Cache: 512kB
Reference: https://www.terraelectronica.ru/pdf/show?pdf_file=%252Fds%252Fpdf%252FT%252FTechicRP3.pdf

### Lab 1
Q:
Is it reasonable to accelerate booting speed by parallel programming during the initialization stage?
A:


Q:
Point out the difference between bare-metal programming and programming on top of operating system.
A:
需要系統幫忙的library就不能使用，像是malloc等分配記憶體空間的函數，或是fork等

### Lab 2
Q1:
In x86 machine, how the above 4 steps are implemented? Roughly describe it.
A1:


Q2:
Calculate how long will it take for loading a 10MB kernel image by UART if baud rate is 115200?
A2: 7281.77s
baud rate 115200 = 115200 bit/s = 14400 byte/s
10 MB = 10 * 1024 * 1024 byte
10 MB / 1440 byte = 10 * 1024 * 1024 / 1440 = 7281.777

### Lab 8

#### question 2 How to implement a component cache mechanism if a file has multiple names (because of hard links)


## How to build

make

## How to run

make run

## How to burn it into pi3

`make`
You can get kernel8.img, then copy to your sd card.


## Architecture

### OS
#### Standard Library
system.h
math.h
string.h

#### Driver
gpio.h
uart.h

#### Shell
pcsh.h

#### Main
main.c

### Script
script.py
test_cmd.txt


## Directory structure

**WIP**
