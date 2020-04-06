# My OSDI 2020

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`0866002`| `JackGrence` | `莊人傑` | green1181772@gmail.com |

## How to build

```
make
```

## How to run

```
make qemu
```

### for debug

```
make debug
```

## How to burn it into pi3

**WIP**

## Architecture

![architecture figure]()

## Directory structure

```
.
├── include		# common used headers
│   └── *.h
├── kernel		# kernel main
│   ├── boot.S
│   ├── linker.ld
│   ├── *.h
│   └── *.c
├── lib			# some useful stuff
│   ├── *.h
│   └── *.c
├── peripheral		# ARM peripherals
│   ├── *.h
│   └── *.c
├── Makefile
├── LICENSE
└── README.md		# this file
```
