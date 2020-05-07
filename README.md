# My OSDI 2020 - LAB 04

## Author

| Student ID | GitHub Account | Name | Email                      |
| -----------| -------------- | ---- | -------------------------- |
| 0856009    | JingWangTW     | 王靖  | jingwang.cs08g@nctu.edu.tw |

## LAB 4 Project

### How to build
```bash
make
```

### Run on QEMU
```bash
make run 
```

## Directory structure
```
.
├── LICENSE
├── Makefile
├── README.md
├── include
│   ├── command.h
│   ├── shell.h
│   └── test_task.h
├── kernel
│   ├── boot.S                  # booting setup
│   ├── exception
│   │   ├── exception.S
│   │   ├── exception.c         # exception controller
│   │   ├── exception.h
│   │   ├── irq.S
│   │   ├── irq.c               # interrupt controller
│   │   ├── irq.h
│   │   ├── sys_call_def.h      # system call type define
│   │   ├── timer.c             # system timer hanlder
│   │   ├── timer.h
│   │   ├── utility.c           # some exception relative utility functions
│   │   └── utility.h
│   ├── peripherals
│   │   ├── gpio.h
│   │   ├── mailbox.c
│   │   ├── mailbox.h
│   │   ├── time.c
│   │   ├── time.h
│   │   ├── uart.c              # uart handler
│   │   └── uart.h
│   └── task
│       ├── mem.c               # memory allocator
│       ├── mem.h
│       ├── schedule.S
│       ├── schedule.c          # scheduling controller
│       ├── schedule.h
│       ├── task.S
│       ├── task.c              # task(thread) creater, hanlder
│       ├── task.h
│       ├── task_queue.c        # task(thread) queue controller
│       └── task_queue.h
├── kernel8.img
├── lib                         # some system libraries
│   ├── ctype.c                 # define some common type
│   ├── ctype.h
│   ├── io.S
│   ├── io.c                    # io, printf, gets
│   ├── io.h
│   ├── math.c
│   ├── math.h
│   ├── stdarg.h
│   ├── string.c
│   ├── string.h
│   ├── sys.S                   # function to get some system info
│   ├── sys.h
│   ├── task.S
│   ├── task.c                  # idle(), zombie, reaper
│   ├── task.h
│   ├── time.S                  # wait, get_time_stamp
│   ├── time.h
│   └── type.h
├── link.ld
└── src
    ├── command.c
    ├── main.c
    ├── shell.c
    └── test_task.c             # task testing functions
```