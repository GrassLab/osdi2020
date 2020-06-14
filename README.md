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
.
├── LICENSE
├── Makefile
├── README.md
├── include
│   ├── allocator.h     
│   ├── command.h
│   ├── ctype.h
│   ├── gpio.h
│   ├── mailbox.h
│   ├── math.h
│   ├── mem.h           
│   ├── shell.h
│   ├── stdarg.h
│   ├── string.h
│   ├── time.h
│   ├── tmpfs.h         
│   ├── type.h
│   ├── uart.h
│   └── vfs.h           
├── link.ld
└── src
    ├── allocator.c
    ├── command.c
    ├── ctype.c
    ├── mailbox.c
    ├── main.c          
    ├── math.c
    ├── mem.c
    ├── shell.c
    ├── start.S
    ├── string.c        
    ├── time.c
    ├── tmpfs.c         
    ├── uart.c
    └── vfs.c           
```