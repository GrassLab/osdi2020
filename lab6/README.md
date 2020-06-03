# NCTU OSDI LAB6
## 

| student id  | name  | lab  |
|---|---|---|
|  0510507 | 吳政緯  | 6  |

### Directory Tree

```
└── src
    ├── boot.S
    ├── config.txt
    ├── entry.S
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
    ├── sys.c
    ├── timer.c
    ├── timer.S
    ├── user_code.S
    ├── user_program
    └── utils.S
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
