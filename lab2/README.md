# BootLoader

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

## Directory structure

```
.
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
├── Makefile
├── README.md
└── src
    ├── boot.S
    ├── info.c
    ├── io.c
    ├── kernel.c
    ├── lfb.c
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

2 directories, 40 files
```
