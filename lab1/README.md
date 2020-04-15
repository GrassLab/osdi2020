# Hello World

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`0856064`| `nobodyzxc` | `陳子軒` | nobodyzxc.tw@gmail.com |

## How to build

```bash
make
```

## How to run

```
make test
```

## How to burn it into pi3

```
make update
```

## Directory structure

```
.
├── include
│   ├── gpio.h
│   ├── io.h
│   ├── loadimg.h
│   ├── map.h
│   ├── mbox.h
│   ├── mm.h
│   ├── power.h
│   ├── shell.h
│   ├── sprintf.h
│   ├── string.h
│   ├── test.h
│   ├── time.h
│   ├── uart.h
│   └── util.h
├── Makefile
├── README.md
└── src
    ├── boot.S
    ├── io.c
    ├── kernel.c
    ├── linker.ld
    ├── loadimg.c
    ├── mbox.c
    ├── mm.S
    ├── power.c
    ├── shell.c
    ├── sprintf.c
    ├── string.c
    ├── test.c
    ├── time.c
    ├── uart.c
    └── util.S

2 directories, 31 files
```
