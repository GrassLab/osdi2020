#!/usr/bin/python3
import serial
import time

s = serial.Serial("/dev/ttyUSB0", 115200)
f = open("command.txt", "r")
cmd = f.readline()
while cmd:
    print("cmd>>", cmd.strip())
    s.write(cmd.encode()+b'\n')
    time.sleep(1)

    while s.in_waiting:
        res = s.readline().decode()
        res = res.strip()
        print(res)
    
    cmd = f.readline()