import serial
import time
import os

s = serial.Serial(
   port='/dev/ttyUSB0',\
   baudrate=115200
   )

kernel = "../kernel8.img"
size = os.path.getsize(kernel)

s.write(size.to_bytes(4, 'little'))
time.sleep(1)

with open(kernel, "rb") as f:
    write_len = s.write(f.read())

print("write %d bytes" % write_len)