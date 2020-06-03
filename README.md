# My OSDI 2020 - LAB 06

## Author

| Student ID | GitHub Account | Name | Email                      |
| -----------| -------------- | ---- | -------------------------- |
| 0856009    | JingWangTW     | 王靖  | jingwang.cs08g@nctu.edu.tw |

## LAB 6 Project

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
.
├── LICENSE
├── Makefile                # make file
├── README.md
├── include                 # header files
│   ├── allocator.h         # header file for allocator.c
│   ├── command.h
│   ├── ctype.h
│   ├── framebuffer.h
│   ├── gpio.h
│   ├── img_data.h
│   ├── mailbox.h
│   ├── math.h              # header file for math.c
│   ├── mem.h               # header file for mem.c
│   ├── shell.h
│   ├── stdarg.h            # define stdarg
│   ├── string.h
│   ├── time.h
│   ├── type.h              # define some common type
│   └── uart.h
├── link.ld
└── src                     # source files
    ├── allocator.c         # register for fixed-size, vaired-size allocator, allocate and free memory from the both allocator
    ├── command.c
    ├── ctype.c
    ├── framebuffer.c
    ├── mailbox.c
    ├── main.c
    ├── math.c
    ├── mem.c               # buddy system
    ├── shell.c
    ├── start.S             # startup booting process
    ├── string.c
    ├── time.c
    └── uart.c
```