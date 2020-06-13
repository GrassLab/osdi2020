# My OSDI 2020 - LAB 07

## Author

| Student ID | GitHub Account | Name | Email                      |
| -----------| -------------- | ---- | -------------------------- |
| 0856009    | JingWangTW     | 王靖  | jingwang.cs08g@nctu.edu.tw |

## LAB 7 Project

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
│   ├── framebuffer.h
│   ├── gpio.h
│   ├── img_data.h
│   ├── mailbox.h
│   ├── math.h              
│   ├── mem.h               
│   ├── shell.h
│   ├── stdarg.h            
│   ├── string.h
│   ├── time.h
│   ├── type.h              
│   └── uart.h
├── link.ld
└── src                     
    ├── allocator.c         
    ├── command.c
    ├── ctype.c
    ├── framebuffer.c
    ├── mailbox.c
    ├── main.c
    ├── math.c
    ├── mem.c               
    ├── shell.c
    ├── start.S             
    ├── string.c
    ├── time.c
    └── uart.c
```