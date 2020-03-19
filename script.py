import string
import serial
import time
from array import array

#Plaintxt.txt File
with open('./test_cmd.txt') as f:
    content = f.readlines()
    print(content)


PORT = '/dev/ttyUSB0'
PORT = '/dev/pts/7'

BAUD_RATES = 115200

ser = serial.Serial(PORT, BAUD_RATES)
 
ser.flush()
ser.flushInput()
ser.flushOutput()

try:
    for line in content:
        delay_time = 0.3
        if line == 'reboot\n':
            delay_time = 1.0
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



finally:
    ser.close()
