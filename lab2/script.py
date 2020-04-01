import serial
import time
import os 
import sys

def shell(ser):
    while True:
        cmd = input() + '\n'
        ser.write(cmd.encode())
        if cmd == 'loadimg\n':
            loadimg(ser)
        else:
            data = ser.read_until(terminator = b'# ').decode()
            print('Pi:', data, end = '')

def loadimg(ser):
    #data = ser.read_until(terminator = b']').decode()
    #print('entry:', data, end='')
    print('wait pi response')
    print('Pi:', ser.read_until(terminator=b'PleaseLoadimg').decode())
    
    # read file size
    with open(sys.argv[1], 'rb') as f:
        data = f.read()
    size  = len(data)
   
    # send kerinel size to RPi
    line = str(size) + '\n'
    ser.write(line.encode())
    print('wait pi response')
    print('Pi:', ser.read_until(terminator = b'RecvSizeDone').decode())
   
    #read file
    data = list()
    with open(sys.argv[1], 'rb') as f:
        while True:
            b = f.read(1)
            if not b:
                break
            else:
                data.append(b) 
    #send file
    ser.write(b''.join(data))

    print('wait pi response')
    print('Pi:', ser.read_until(terminator = b'RecvImgDone').decode())
    

ser = serial.Serial(
   port = sys.argv[2],\
   baudrate=115200
   )

try:
    shell(ser)
    
except PermissionError as e:
    print("Check your permission!");
