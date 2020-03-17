#!/usr/bin/python3
import serial
import sys
import time
def sendData(data):
    data += "\n"
    ser.write(data.encode())

ser = serial.Serial("/dev/ttyUSB0",115200)
with open(sys.argv[1]) as f:
    content = f.readlines()
for command in content:
    time.sleep(0.008)
    ser.write(command.encode())

