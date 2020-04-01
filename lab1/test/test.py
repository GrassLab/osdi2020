import serial
import time

ser = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate=115200)

try:
    with open('./test.txt') as f:
        line = f.readline()
        while line:
            ser.write(line.encode())
            print(ser.readline().decode('utf-8'))
            
            print(ser.readline().decode('utf-8'))
            ser.flushOutput()
            line = f.readline()
            time.sleep(0.1)
except PermissionError as e:
        print("ERROR!")
