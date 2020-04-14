# My OSDI 2020 - LAB 03

## Author

| Student ID | GitHub Account | Name | Email                      |
| -----------| -------------- | ---- | -------------------------- |
| 0856009    | JingWangTW     | 王靖  | jingwang.cs08g@nctu.edu.tw |

## LAB 3 Project

### How to build
```bash
make
```

### Run on QEMU
```bash
make run 
```

### Simple Shell
| command       | description                       | 
| --------------| --------------------------------- | 
| hello         | print Hello World!                |
| help          | print all available commands      |
| timestamp     | print current timestamp           |
| reboot        | reset rpi3                        |
| vc_base_addr  | print vc core base address        |
| board_revision| print boarf revision              |
| exc           | trap into el1                     |
| hvc           | trap into el2                     |
| timer         | enable core and local timer intr  |
| timer-stp     | disable core and local timer intr |

## Directory structure

```
.
├── LICENSE
├── Makefile
├── README.md
├── include                 # header file of code in src
│   ├── command.h
│   ├── img_data.h
│   └── shell.h
├── kernel                  
│   ├── exception           # code associated with exception
│   │   ├── exception.S     # exception handler
│   │   ├── exception.c     # exception controller
│   │   ├── exception.h
│   │   ├── irq.S           # interript handler
│   │   ├── irq.c           # interrupt controller
│   │   ├── irq.h
│   │   ├── timer.c         # code to controller core and local timer
│   │   ├── timer.h
│   │   ├── utility.c       # exception utility function
│   │   └── utility.h
│   ├── peripherals         # code to deal with peripher
│   │   ├── gpio.h
│   │   ├── mailbox.c
│   │   ├── mailbox.h
│   │   ├── time.c
│   │   ├── time.h
│   │   ├── uart.c
│   │   └── uart.h          # uart_printf
│   └── start.S             # booting process
├── lib                     # some standard function and definition
│   ├── ctype.c
│   ├── ctype.h
│   ├── math.c
│   ├── math.h
│   ├── stdarg.h
│   ├── string.c
│   ├── string.h
│   └── type.h
├── link.ld                 # linker file
└── src                     # code to some application
    ├── command.c
    ├── main.c
    └── shell.c
```