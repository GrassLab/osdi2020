import serial
import time

ser = serial.Serial(
   port='/dev/ttyUSB0',\
   baudrate=115200
   )

try:
    with open('../cmd.txt') as f:
        line = f.readline()
        while line:
            ser.write(line.encode())
            line = f.readline()
            time.sleep(0.1)
except PermissionError as e:
    print("Check your permission!");
