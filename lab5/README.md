# Virtual memory

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`0856064`| `nobodyzxc` | `陳子軒` | nobodyzxc.tw@gmail.com |

## How to build

```
make
```

## How to run

```
make run
```

## How to burn it into pi3

```
make update
```

## Directory structure

```
.
├── debug
│   ├── cmd.gdb
│   ├── kernel.gdb
│   └── user.gdb
├── include
│   ├── entry.h
│   ├── gpio.h
│   ├── info.h
│   ├── io.h
│   ├── irq.h
│   ├── lfb.h
│   ├── loadimg.h
│   ├── map.h
│   ├── mbox.h
│   ├── mm.h
│   ├── mmu.h
│   ├── mutex.h
│   ├── power.h
│   ├── sched.h
│   ├── shell.h
│   ├── sprintf.h
│   ├── string.h
│   ├── syscall.h
│   ├── sys.h
│   ├── sysregs.h
│   ├── task.h
│   ├── test.h
│   ├── time.h
│   ├── timer.h
│   ├── uart.h
│   └── util.h
├── Makefile
├── README.md
├── src
│   ├── boot.S
│   ├── entry.S
│   ├── exception.c
│   ├── info.c
│   ├── io.c
│   ├── irq.c
│   ├── kernel.c
│   ├── lfb.c
│   ├── linker.ld
│   ├── loadimg.c
│   ├── mbox.c
│   ├── mini.c
│   ├── mm.c
│   ├── mm.S
│   ├── pl011.c
│   ├── power.c
│   ├── sched.c
│   ├── sched.S
│   ├── shell.c
│   ├── sprintf.c
│   ├── string.c
│   ├── sys.c
│   ├── syscall.S
│   ├── task.c
│   ├── template.ld
│   ├── test.c
│   ├── time.c
│   ├── timer.c
│   ├── timer.S
│   └── util.S
└── usrc
    ├── entry.S
    ├── linker.ld
    ├── main.c
    ├── Makefile
    ├── mutex.h -> ../include/mutex.h
    ├── shell.c
    ├── shell.h
    ├── sprintf.c
    ├── sprintf.h
    ├── string.c
    ├── string.h
    ├── syscall.h -> ../include/syscall.h
    ├── syscall.S -> ../src/syscall.S
    ├── test.c
    ├── test.h
    ├── usrlib.h
    └── util.h

4 directories, 78 files
```
