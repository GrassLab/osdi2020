# My OSDI 2020 - LAB 02

## Author

| Student ID | GitHub Account | Name | Email                      |
| -----------| -------------- | ---- | -------------------------- |
| 0856009    | JingWangTW     | 王靖  | jingwang.cs08g@nctu.edu.tw |

## Directory structure

```
.
├── include             # header files
│   ├── command.h       # header file to process command
│   ├── ctype.h         # header file to implement some funcion in <ctype.h>
│   ├── framebuffer.h   # header file to process framebuffer relative function
│   ├── gpio.h          # header file to define some constant address
│   ├── img_data.h      # header file of image data
│   ├── mailbox.h       # header file to process mailbox
│   ├── math.h          # header file to implement some function in <math.h>
│   ├── shell.h         # header file to process shell flow
│   ├── string.h        # header file to implement some function in <string.h>
│   ├── time.h          # header file to process time relative function
│   ├── type.h          # header file to define some type like in <type.h>
│   └── uart.h          # header file to process uart interface
├── LICENSE
├── link.ld
├── Makefile
├── raspbootocm.cc      # code for sending kernel image
├── README.md
└── src                 # source files
    ├── command.c       # source file to process command
    ├── ctype.c         # source file to implement some funcion in <ctype.h>
    ├── framebuffer.c   # source file to process framebuffer relative function
    ├── mailbox.c       # source file to process mailbox
    ├── main.c          # int main
    ├── math.c          # source file to implement some function in <math.h>
    ├── shell.c         # source file to process shell flow
    ├── start.S         # source code for booting
    ├── string.c        # source file to implement some function in <string.h>
    ├── time.c          # source file to process time relative function
    └── uart.c          # source file to process uart interface


```

## How to build
```bash
make
```

## Run on QEMU
```bash
make run
```

## Simple Shell
| command       | description                   | 
| --------------| ----------------------------- | 
| hello         | print Hello World!            |
| help          | print all available commands  |
| timestamp     | print current timestamp       |
| reboot        | reset rpi3                    |
| vc_base_addr  | print vc core base address    |
| board_revision| print boarf revision          |
| loadimg       | load kernel image             |

## Send kernel image

* Step 1. Compile code
    ```bash
    gcc raspbootocm.cc -o raspbootocm
    ```

* Step 2. Run with follow patterm
    ```bash
    ./raspbootocm <dev> <kernel image>
    ```