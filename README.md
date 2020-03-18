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
├── labX		# labX (X = 0 ~ 8)
│   ├── boot.S		# source code for boot
│   ├── README.md	# some notes for labX
│   ├── linker.ld	# linker script
│   └── Makefile
├── LICENSE
└── README.md		# this file
```
