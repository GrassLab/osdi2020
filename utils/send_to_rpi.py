#!/usr/bin/env python

import os
import time
import serial

try:
    if os.geteuid() != 0:
        raise RuntimeError("This script must be run as root")
    
    os.chmod('/dev/ttyS5', 0o666)
    s = serial.Serial(
        port='/dev/ttyS5',
        baudrate=115200,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
    )
   
    s.flush()
    s.flushInput()
    s.flushOutput()
    path = "./output/kernel8.img"
    size = os.path.getsize(path)
    print(f"image size: {size}")
    s.write(size.to_bytes(4, 'big'))
    with open(path, 'rb') as file:
        s.write(file.read())
        exit()

except Exception as e:
    print(f"[EXCEPTION] {e}")

