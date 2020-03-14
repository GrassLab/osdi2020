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

    with open("cmds", 'rb') as file:
        for line in file:
            line = line.replace(b'\n', b'\r')
            print(f"Sending : {line}")
            s.write(line)
            time.sleep(1)

except Exception as e:
    print(f"[EXCEPTION] {e}")

