# My OSDI 2020

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`0856126`| `ljs7619480` | `李家森` | jiasen[dot]cs08g[at]nctu[dot]edu[dot]tw |

## Requires
This project is test under ubuntu 18.04 LTS. and `Raspberry Pi 3B+`

- Cross-complier
    - For ubuntu pc, we can direcly install by package 
        ```
        $ sudo apt install gcc-aarch64-linux-gnu
        ```
    - If you want to built from source code, you can refer to this [tutorial](https://github.com/bztsrc/raspi3-tutorial/tree/master/00_crosscompiler)

- Emulator [Qemu](https://www.qemu.org/download/#source)
    - This project wa test on Qemu 4.2.0.
    - It is recommend to build qemu by source code because of that the version download by "Advanced Packaging Tools (`APT`)" did not support the model `-M raspi3` when I worked on this project.

## How to build

```
$ make
```

## How to run

```
$ make run
```

## How to test
- test miniUART
    ```
    $ sudo screen /dev/ttyUSB0 115200
    ```
- test by python3 srcipt
    ```
    $ sudo python3 txt2uart.py --filename testfile.txt --device /dev/ttyUSB0 
    ```
    - The python3 code is depend on pyserial module which can install py 
        ```
        pip3 install pyserial
        ```
    - To check more opctions type 
        ```
        python3 txt2uart.py -h
        ```