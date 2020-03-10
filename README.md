# OSDI Lab

* This repo is for OSDI 2020 lab0

## Requirements

* aarch64-toolchains
* qemu-system-aarch64

## Build Instruction

```Bash
$ make -Csrc clean
$ make -Csrc kernel8.img
```

## Run on qemu

```Bash
$ make -Csrc test
```

## Run on rpi3

To be continue ...

## Code Structure

```Bash
osdi2020/
├── LICENSE
├── README.md
└── src
    ├── a.o
    ├── a.S
    ├── kernel8.elf
    ├── kernel8.img
    ├── linker.ld
    └── Makefile
```
