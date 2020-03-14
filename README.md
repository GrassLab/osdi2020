# My OSDI 2020

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`0850739`| `john0829` | `湯忠禮` | joh860829nny@gmail.com |

## How to build

cd lab${num}  
aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf a.o  
aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img  

## How to run

qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm  

## How to burn it into pi3

**WIP**

## Architecture

![architecture figure]()

## Directory structure

**WIP**

