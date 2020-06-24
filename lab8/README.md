# NCTU OSDI LAB8
## 

| student id  | name  | lab  |
|---|---|---|
|  0510507 | 吳政緯  | 8  |

### Directory Tree
```
└── src
    ├── boot.S
    ├── config.txt
    ├── entry.S
    ├── fat32.c
    ├── fork.c
    ├── irq.c
    ├── irq.S
    ├── kernel.c
    ├── linker.ld
    ├── mini_uart.c
    ├── mm.c
    ├── mm.S
    ├── printf.c
    ├── sched.c
    ├── sched.S
    ├── sdhost.c
    ├── slab.c
    ├── sys.c
    ├── timer.c
    ├── timer.S
    ├── tmpfs.c
    ├── user_code.S
    ├── user_program
    ├── utils.S
    └── vfs.c
```

### Compiling
The `lab5_user` folder contains the test cases of user program. You have to compile the user program to raw binary and embed the compiled binary to kernel image.

```
$ cd    ./lab5_user/    &   make
```

Move the compiled `user_program` raw binary file into the kernel `src` folder. 

```
$ mv    user_program    ../src
```
Change directory to lab5 folder and make files.

```
$ cd .. &   make
```
## Put kernel8.img into filesystem
### setup loop device for filesystem
```
$ sudo losetup -fP sfn_nctuos.img --show
```
### mount filesystem
```
$ sudo mount -t msdos /dev/<loop dev> -o offset=1048576 ./img
```
### put kernel8.img into filesystem

## Running the tests

### run qemu
```
$ bash  start.sh
```
### run gdb
```
$ make  gdb
```
### recompile
```
$ make clean
```
## flash into sd card
You can use the following command to flash filesystem into you sd card.
```
dd if=nctuos.img of=/dev/sdb
```