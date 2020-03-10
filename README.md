# My OSDI 2020

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`A082533`| `chichenyang` | `楊季蓁` | chichensmile@gmail.com |

## How to build
'''
$sudo pacman -S aarch64-linux-gnu-gcc
$sudo pacman -S qemu-arch-extra

$aarch64-linux-gnu-gcc -c a.S
$aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf a.o
$aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img
'''
## How to run
'''
$qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm
'''
## How to burn it into pi3

**WIP**

