# My OSDI 2020 - LAB 08

## Author

| Student ID | GitHub Account | Name | Email                      |
| -----------| -------------- | ---- | -------------------------- |
| 0856009    | JingWangTW     | 王靖  | jingwang.cs08g@nctu.edu.tw |

## LAB 8 Project

### Build
```bash
make
```

### Run on QEMU
```bash
make run 
```

### Clean and Rebuild Project
```bash
make force
```

### Run with Debugger
```bash
make debug

# on the other shell
aarch64-linux-gnu-gdb
file build/kernel8.elf
directory .
target remote localhost:1234
```

## Directory structure
```
├── include
│   ├── allocator.h
│   ├── command.h
│   ├── ctype.h
│   ├── fs                  # file system related headers
│   │   ├── fat32.h         # file system - fat32
│   │   ├── sdhost.h        # sd card driver
│   │   └── tmpfs.h         # file system - tmpfs
│   ├── gpio.h
│   ├── mailbox.h
│   ├── math.h
│   ├── mem.h
│   ├── shell.h
│   ├── stdarg.h
│   ├── string.h            # string utility functions header
│   ├── time.h
│   ├── type.h
│   ├── uart.h
│   └── vfs.h
├── LICENSE
├── link.ld
├── Makefile
├── README.md
└── src
    ├── allocator.c
    ├── command.c
    ├── ctype.c
    ├── fs
    │   ├── fat32.c         # implement of FAT32
    │   ├── sdhost.c
    │   └── tmpfs.c         # implement of tmpfs
    ├── mailbox.c
    ├── main.c
    ├── math.c
    ├── mem.c
    ├── shell.c
    ├── start.S
    ├── string.c            # string utility functions
    ├── time.c
    ├── uart.c
    └── vfs.c

├── include
│   ├── allocator.h
│   ├── command.h
│   ├── ctype.h
│   ├── fs
│   │   ├── fat32.h
│   │   ├── sdhost.h
│   │   └── tmpfs.h
│   ├── gpio.h
│   ├── mailbox.h
│   ├── math.h
│   ├── mem.h
│   ├── shell.h
│   ├── stdarg.h
│   ├── string.h
│   ├── time.h
│   ├── type.h
│   ├── uart.h
│   └── vfs.h
├── kernel8.img
├── LICENSE
├── link.ld
├── Makefile
├── README.md
├── sample_kernel8.IMG
└── src
    ├── allocator.c
    ├── command.c
    ├── ctype.c
    ├── fs
    │   ├── fat32.c
    │   ├── sdhost.c
    │   └── tmpfs.c
    ├── mailbox.c
    ├── main.c
    ├── math.c
    ├── mem.c
    ├── shell.c
    ├── start.S
    ├── string.c
    ├── time.c
    ├── uart.c
    └── vfs.c
```