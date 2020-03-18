# Setup

This directory contains the basic procedure of running code on qemu

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
To be continued ...

## Code Structure

```Bash
src
├── a.S
├── linker.ld
└── Makefile
```
