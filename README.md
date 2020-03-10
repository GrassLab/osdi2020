# My OSDI 2020

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`0856539`| `briansp8210` | `李健瑀` | briantaipei8210@gmail.com |

## How to build

`make`

## How to run

`qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm`

## How to burn it into pi3

* Use `dd` to burn the image to pi3

## Architecture

**WIP**

## Directory structure

```
.
├── LICENSE
├── linker.ld
├── Makefile
├── nctuos.img
├── README.md
└── start.S
```
