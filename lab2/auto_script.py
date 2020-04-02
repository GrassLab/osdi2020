#!/usr/bin/python3
import serial
import sys
import time


ser = serial.Serial(sys.argv[2],115200)
k = b""
with open(sys.argv[1], "rb") as f:
    byte = f.read(1)
    k += byte
    while byte != b"":
        byte = f.read(1)
        k += byte
print(len(k))
ser.write(int(0x60000).to_bytes(4, 'little'))
ser.write(len(k).to_bytes(4, 'little')+k)

