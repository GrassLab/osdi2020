# Setup

This directory contains the basic procedure of running code on qemu

## Author

| Student ID | GitHub Account | Name | Email |
| ------- | ----------- | ------- | ------------------- |
|`A082647`| `zlsh80826` | 王宇正 | zlsh80826@gmail.com |

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
