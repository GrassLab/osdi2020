import string
import serial
import time
import os
from array import array

PORT = '/dev/pts/3'
PORT = '/dev/ttyUSB0'
BAUD_RATES = 115200

ser = serial.Serial(PORT, BAUD_RATES)

ser.flush()
ser.flushInput()
ser.flushOutput()

content = ["hello\n", "help\n"]

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
        print("========")
        time.sleep(delay_time)

    time.sleep(3)

finally:
    ser.close()