import string
import serial
import time
import os
from array import array



#PORT = '/dev/ttyUSB0'
PORT = '/dev/pts/5'
BAUD_RATES = 115200

ser = serial.Serial(PORT, BAUD_RATES)
 
ser.flush()
ser.flushInput()
ser.flushOutput()

kernel_path = "./other/kernel8_80100.img"
kernel_size = os.path.getsize(kernel_path)

content = ["loadimg\n", "90000\n", str(kernel_size)+"\n"]
try:
    for line in content:
        delay_time = 1.
        ser.flush()
        ser.flushInput()
        ser.flushOutput()

        print(line)
        ser.write(line.encode())

        time.sleep(delay_time)

        count = ser.in_waiting
        if count != 0:
            data_raw = ser.read(count)
            print(data_raw.decode())
            ser.flushInput()
        input('aaa')
        time.sleep(delay_time)

    time.sleep(3)

    # send kernel images
    ser.flush()
    ser.flushInput()
    ser.flushOutput()

    index = 1
    checksum = 0
    with open(kernel_path, "rb") as f:
        byte = f.read(1)
        while byte:
            checksum += int.from_bytes(byte, byteorder='big')
            print(str(index))
            index += 1
            ser.write(byte)
            byte = f.read(1)
            time.sleep(0.01)
    count = ser.in_waiting
    if count != 0:
        data_raw = ser.read(count)
        print(data_raw.decode())
        ser.flushInput()
    input('aaa')
    time.sleep(delay_time)
    print("sendend")
    input('aaa')
    content = ["help\n"]
    
    for line in content:
        delay_time = 1.
        ser.flush()
        ser.flushInput()
        ser.flushOutput()

        print(line)
        ser.write(line.encode())

        time.sleep(delay_time)

        count = ser.in_waiting
        if count != 0:
            data_raw = ser.read(count)
            print(data_raw.decode())
            ser.flushInput()
        time.sleep(delay_time)


finally:
    ser.close()