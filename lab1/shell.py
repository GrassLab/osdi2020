import serial
import time

s = serial.Serial(
   port='/dev/ttyUSB0',\
   baudrate=115200
   )

with open('cmd') as f:
    line = f.readline()
    while line:
        s.write(line.encode())
        time.sleep(1.5)
        line = f.readline()