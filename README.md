# My OSDI 2020 - LAB 02

## Author

| Student ID | GitHub Account | Name | Email                      |
| -----------| -------------- | ---- | -------------------------- |
| 0856009    | JingWangTW     | 王靖  | jingwang.cs08g@nctu.edu.tw |

## LAB 2 Project

### How to build
```bash
make
```

### Run on QEMU
```bash
make run 
```

### Simple Shell
| command       | description                   | 
| --------------| ----------------------------- | 
| hello         | print Hello World!            |
| help          | print all available commands  |
| timestamp     | print current timestamp       |
| reboot        | reset rpi3                    |
| vc_base_addr  | print vc core base address    |
| board_revision| print boarf revision          |

### How to run
It should run with the sender below

## Prepare image burn on sd card
* Step1. Build image under `loader` directory.
    ```bash
    # done this under the loader directory (eg. cd loader)
    make
    ```

* Step 2. Copy image file under the loader directory to the SD card.
    ```bash
    # done this in the root directory
    cp ./loader/kernel8.img <SD Card>
    ```
* Step 3. Build host sender program.
    ```bash
    # done this in the root directory
    make loader
    ```

* Step 4. Run the host code
    * kernel image sould be the built by the [previous step](#LAB-2-Project).
    ```bash
    # done this in the root directory
    ./raspbootocm <dev> ./kernel8.img  
    ```

*  Therefore, there is a image in sd card that will always load image from uart when booting.

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
├── link.ld             # linker script
├── loader              # loaders relative files
│   ├── kernel8.img     # pre-compiler image 
│   ├── Makefile
│   ├── raspbootocm.cc  # source code to run host to send image to rpi3 by UART
│   └── src             # source files
│       ├── gpio.h
│       ├── link.ld
│       ├── main.c
│       ├── mbox.c
│       ├── mbox.h
│       ├── start.S
│       ├── uart.c
│       └── uart.h
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