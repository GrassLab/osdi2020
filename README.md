# My OSDI 2020

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

## Architecture

WIP

## Directory structure

```
.
├── bin
│   ├── rpish
│   └── test.sh
├── CMakeLists.txt
├── include
│   ├── io.h
│   ├── map.h
│   ├── mini_uart.h
│   ├── mm.h
│   ├── power.h
│   ├── shell.h
│   ├── string.h
│   ├── time.h
│   └── util.h
├── LICENSE
├── Makefile
├── README.md
└── src
    ├── boot.S
    ├── CMakeLists.txt
    ├── io.c
    ├── kernel.c
    ├── linker.ld
    ├── mini_uart.c
    ├── mm.S
    ├── power.c
    ├── shell.c
    ├── string.c
    ├── time.c
    └── util.S

3 directories, 28 files
```
