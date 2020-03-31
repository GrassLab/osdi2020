# My OSDI 2020

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`0856064`| `nobodyzxc` | `陳子軒` | nobodyzxc.tw@gmail.com |

## How to build

bootloader@0x80000 

```bash
make
```

kernel@0x20000 with timestamp

```bash
make stamp
```

kernel@0x40000 with timestamp
```
make stamp
```

## How to run

```
make test
```

## How to burn it into pi3

```
make update
```

## Architecture

WIP

## Directory structure

```
.
├── bin
│   ├── chpi.jpg
│   ├── img2clang.py
│   ├── movpi.jpg
│   ├── Raspberry-Pi-Logo.jpg
│   ├── rpish
│   ├── test.sh
│   └── trpi.jpg
├── echo.img
├── include
│   ├── gpio.h
│   ├── homer.h
│   ├── info.h
│   ├── io.h
│   ├── lfb.h
│   ├── loadimg.h
│   ├── map.h
│   ├── mbox.h
│   ├── mm.h
│   ├── movpi.h
│   ├── power.h
│   ├── raspi.h
│   ├── shell.h
│   ├── sprintf.h
│   ├── string.h
│   ├── test.h
│   ├── time.h
│   ├── uart.h
│   └── util.h
├── LICENSE
├── Makefile
├── README.md
└── src
    ├── boot.S
    ├── info.c
    ├── io.c
    ├── kernel.c
    ├── lfb.c
    ├── linker.ld
    ├── loadimg.c
    ├── mbox.c
    ├── mini.c
    ├── mm.S
    ├── pl011.c
    ├── power.c
    ├── shell.c
    ├── sprintf.c
    ├── string.c
    ├── template.ld
    ├── test.c
    ├── time.c
    └── util.S

3 directories, 49 files
```
