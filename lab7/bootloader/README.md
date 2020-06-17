# README
+ please use auto_script.py in folder "lab2" to send your kernel image

```shell=
sudo python3 auto_script.py [path/to/your/kernel8.im] [path/to/your/ttyUSB0]
```

+ first: send 4 bytes address you want to put in (note: don't put in address 0x7F800 ~ 0x80000)
+ second: send 4 bytes size of kerel8.img 
+ thrid: send your kernel8.ing byte by byte