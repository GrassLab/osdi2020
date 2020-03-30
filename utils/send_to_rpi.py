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

    with open("kernel8.img", 'rb') as file:
        position = 0
        size = file.tell()
        byte = file.read(1)
        while byte != b"":
            #for line in file:
            #line = line.replace(b'\n', b'\r')
            file.seek(position, 0)
            byte = file.read(1)
            print(f"Sending : {byte}")
            s.write(byte)
            position = position + 1
            #time.sleep(1)

except Exception as e:
    print(f"[EXCEPTION] {e}")

