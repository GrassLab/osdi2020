# My OSDI 2020 Lab1

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`A082533`| `chichenyang` | `楊季蓁` | chichensmile@gmail.com |

## Goals:
1.  set up mini UART and let the host computer and rpi3 communicate through it.
2.  implement a simple shell to let rpi3 interact with the host computer. 

## Simple Shell
Support the following commands:

| Command | Description|
|---------|------------|
|help|print all available commands|
|hello|print Hello World|
|timestamp|print current timestamp (timer counter / timer frequency)|
|reboot| reset on real rpi3, not on QEMU.|

## Script
We provide a script which can read a text file (cmd.txt) on host machine and write the content to rpi3.
Note: You have installed "serial" python package and have root permission. 
```
    python script.py
```

## How to build
```
    make
```

## How to run on qemu
```
    make run
```

## How to interact with rpi3
```
    screen /dev/ttyUSB0 115200
```

