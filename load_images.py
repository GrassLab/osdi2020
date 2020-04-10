import string
import serial
import time
import os
from array import array



PORT = '/dev/ttyUSB0'
PORT = '/dev/pts/14'

BAUD_RATES = 115200

ser = serial.Serial(PORT, BAUD_RATES)
 
ser.flush()
ser.flushInput()
ser.flushOutput()

kernel_path = './other_kernels/kernel8.img'
#kernel_path = './other_kernels/kernel8_2.img'
#kernel_path = './kernel8.img'
kernel_size = os.path.getsize(kernel_path)
# 0x80000 = 524288
content = ["load_images\n", str(kernel_size)+"\n", "524288\n"]
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
            print(data_raw)

            ser.flushInput()
        print("========")
        time.sleep(delay_time)

    time.sleep(3)

    # send kernel images
    ser.flush()
    ser.flushInput()
    ser.flushOutput()

    index = 1
    with open(kernel_path, "rb") as f:
        byte = f.read(1)
        while byte:
            print(str(index))
            index += 1
            ser.write(byte)
            byte = f.read(1)

            time.sleep(0.0001)
            # check recv
            count = ser.in_waiting
            if count != 0:
                data_raw = ser.read(count)
                print("=", data_raw)

    
    time.sleep(3)

    count = ser.in_waiting
    if count != 0:
        data_raw = ser.read(count)
        print(data_raw)
    
    ser.flush()
    ser.flushInput()
    ser.flushOutput()

finally:
    ser.close()
