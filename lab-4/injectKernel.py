from __future__ import print_function
import serial
import time
import sys


"""
sudo python injectKernel.py ./kernel8.img /dev/ttyUSB0 80000
"""

sleepTime = 0.1
kernelPath = sys.argv[1] # './kernel8.img'
device = sys.argv[2] # '/dev/ttyUSB0'
loadAddres = sys.argv[3] # '80000'
# device = '/dev/pts/0'

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
    chunkSize = 512
    resetSerialIO(serialObject)
    for i in range(len(data)/chunkSize + 1):
        time.sleep(sleepTime)
        if ((chunkSize * (i+1)) < len(data)):
            ser.write(data[i*chunkSize : (i+1)*chunkSize])
        else:
            ser.write(data[ i*chunkSize: ])

serialObject = serial.Serial(device, 115200, timeout=0.001)
resetSerialIO(serialObject)
serialSend(serialObject, loadAddres + '\n')
serialSend(serialObject, str(len(readKernel(kernelPath))) + '\n')
serialSendChunk(serialObject, readKernel(kernelPath))