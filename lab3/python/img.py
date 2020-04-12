import ctypes
from bitstring import BitArray
import os
import serial
import argparse
import time

# kernel_path = "../../kernel8.img"
# kernel_path = "../../nctuos.img"
kernel_path = "../../lab2/test_kernel/kernel8_yang.img"


path = "/dev/ttyUSB0"
# path = "/dev/pts/9"

file_size = os.path.getsize(kernel_path)
print(file_size)


ser=serial.Serial(path,115200,timeout=0.5) 
ser.write(str.encode("send\n"))
time.sleep(0.2)
for i in str(file_size):
    ser.write(str.encode(i))
    time.sleep(0.1)
ser.write(str.encode("\n"))
time.sleep(0.2)

time.sleep(3)

count = 0
c = 0
with open(kernel_path, "rb") as f:
    byte = f.read(1)
    c += (int.from_bytes(byte, byteorder='big'))
    while byte:
        time.sleep(0.0001)
        ser.write(byte)
        if count % 1000 == 0:
            print("---now count",count)
        count += 1
        byte = f.read(1)
        c += (int.from_bytes(byte, byteorder='big'))
print(byte)
print(c)


ser.close()
# while 1:
    # x = ser.readline()
    # print(x)



'''

c = 0
with open("kernel8.img", "rb") as f:
    byte = f.read(1)
    while byte != b"":
        # Do stuff with byte.
        byte = f.read(1)
        if c < 100:
            print(byte)
        c+=1
'''
