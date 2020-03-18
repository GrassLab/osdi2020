# My OSDI 2020 - LAB 01

## Author

| Student ID | GitHub Account | Name | Email                      |
| -----------| -------------- | ---- | -------------------------- |
| 0856009    | JingWangTW     | 王靖  | jingwang.cs08g@nctu.edu.tw |

## Files
| File          | Content                                               | 
| --------------| ----------------------------------------------------- | 
| command.c(.h) | code for action to deal with different shell command  |
| gpio.c        | some gpio config                                      |
| main.c        | main program                                          |
| math.c(.h)    | code for replace standard math.h                      |
| shell.c(.h)   | code for control the shell behave                     |
| string.c(.h)  | code for replace standard math.h                      |
| uart.c(.h)    | code for uart opertaion                               |
| link.ld       | linker script                                         |


## How to build

```bash
make
```

## Run on QEMU
```bash
make run
```

## Simple Shell
| command   | description                   | 
| ----------| ----------------------------- | 
| hello     | print Hello World!            |
| help      | print all available commands  |
| timestamp | print current timestamp       |
| reboot    | reset rpi3                    |