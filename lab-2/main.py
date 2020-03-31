from __future__ import print_function
import serial
import time
import sys

sleepTime = 0.05

def resetSerialIO(s):
    s.flush()
    s.flushInput()
    s.flushOutput()

def serialSend(ser, string):
    resetSerialIO(serialObject)
    ser.write(string)
    time.sleep(sleepTime)
    output = ser.read(ser.in_waiting)
    resetSerialIO(ser)
    if output[0:len(string)-1] == string[:-1]:
        output = output[len(string)+1:]
    print(output, end='')
    time.sleep(sleepTime)

def readKernel(kernelPath):
    file = open(kernelPath)
    kernelContent = file.read()
    file.close()
    return kernelContent

def serialSendChunk(ser, data):
    chunkSize = 512
    resetSerialIO(serialObject)
    count = 0
    for i in range(len(data)/chunkSize + 1):
        time.sleep(sleepTime)
        if ((chunkSize * (i+1)) < len(data)):
            count += len(data[i*chunkSize : (i+1)*chunkSize])
            ser.write(data[i*chunkSize : (i+1)*chunkSize])
        else:
            count += len(data[ i*chunkSize: ])
            ser.write(data[ i*chunkSize: ])
    resetSerialIO(ser)

kernelPath = './kernel8.img'
# serialObject = serial.Serial("/dev/ttyUSB0", 115200, timeout=0.001)
serialObject = serial.Serial("/dev/pts/0", 115200, timeout=0.001)
resetSerialIO(serialObject)

while True:
    command = raw_input() + '\n'
    sleepTime = 0.05
    if command == 'reboot\n':
        sleepTime = 2.0
    serialSend(serialObject, command)

    if command == 'loadimg\n':
        serialSend(serialObject, str(len(readKernel(kernelPath))) + '\n')
        serialSendChunk(serialObject, readKernel(kernelPath))

serialObject.close()