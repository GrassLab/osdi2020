# My OSDI 2020

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`0845204`| `gbrocart` | `Guillaume Brocart` | guillaume.brocart@etu.utc.fr |

## Prerequisites

To build the kernel, you need an aarch64 cross-compiler and a virtual machine to test it.
The following command install the prerequisites on archlinux, but the package names may vary depending on your distribution.
```bash
pacman -S aarch64-linux-gnu-gcc qemu-arch-extra
```

You also need screen to interact with uart and python with pyserial to use the testing script. 
```bash
pacman -S screen python python-pip 
pip install pyserial
```

## How to build

```bash
make
```

## How to run

For assembly output:
```bash
make asmrun
```

For gdb debug:
```bash
make debugrun
```

## How to burn it into pi3

- Format the sdcard in FAT32
- Copy the files 
- Start the pi3
- ```bash sh utils/screen.sh```

## Directory structure

```bash
.
├── boot
│   ├── boot.S
│   └── link.ld
├── lib
│   ├── mm.h
│   ├── shell.h
│   ├── string.h
│   ├── types.h
│   ├── uart.h
│   └── watchdog.h
├── LICENSE
├── Makefile
├── README.md
├── src
│   ├── kernel.c
│   ├── shell.c
│   ├── string.c
│   ├── uart.c
│   └── watchdog.c
└── utils
    ├── cmds
    ├── screen.sh
    └── send_to_rpi.py
```
