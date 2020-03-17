## Simple Shell
A simple shall with several commands

|Command|Description|
|---|---|
|help|show the help message|
|hello|send hello world|
|reboot|reboot the rbpi3|
|uptime|tell how long the system has been running|
|timestamp|alias of uptime|

## Build
```Bash
make
```

## Emulate
```Bash
make run
```

## Run on rbpi3

* Copy the ``kernel8.img`` into SD card with FAT32 partition and necessary firmwares
* Connect the computer with mini UART
* Use any tool to read, write /dev/ttyUSB0

### screen example
```Bash
screen /dev/ttyUSB0 115200
```

## Scripting
We prepare a simple script that can serial execute the command file

### Usage
```Bash
python3 script.py <command file>
```
Command file can reference ``../testcase/cmd.txt``
