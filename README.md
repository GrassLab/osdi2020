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

**TODO**

## Architecture

**TODO**

## Directory structure

```bash
.
├── boot.S
├── LICENSE
├── link.ld
├── Makefile
└── README.md
```
