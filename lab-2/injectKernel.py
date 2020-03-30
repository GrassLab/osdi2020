from __future__ import print_function
import serial
import time
import sys

sleepTime = 0.1

def resetSerialIO(s):
    s.flush()
    s.flushInput()
    s.flushOutput()

def readKernel(kernelPath):
    file = open(kernelPath)
    kernelContent = file.read()
    file.close()
    return kernelContent

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

def serialSendChunk(ser, data):
    resetSerialIO(serialObject)
    count = 0
    for i in range(len(data)/512 + 1):
        time.sleep(sleepTime)
        if ((512 * (i+1)) < len(data)):
            count += len(data[i*512:i*512+512])
            ser.write(data[i*512:i*512+512])
        else:
            count += len(data[i*512:])
            ser.write(data[i*512:])
    print("count = ", count) 
    resetSerialIO(ser)

kernelPath = './bin/kernel8.img'
# serialObject = serial.Serial("/dev/ttyUSB0", 115200, timeout=0.001)
serialObject = serial.Serial("/dev/pts/0", 115200, timeout=0.001)
resetSerialIO(serialObject)

serialSend(serialObject, str(len(readKernel(kernelPath))) + '\n')
serialSendChunk(serialObject, readKernel(kernelPath))