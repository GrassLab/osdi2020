from __future__ import print_function
import serial
import time
import sys

def resetSerialIO(s):
    s.flush()
    s.flushInput()
    s.flushOutput()

filePath = (sys.argv[1])
file = open(filePath)
commands = file.readlines()
file.close()

serialObject = serial.Serial("/dev/ttyUSB0", 115200)
resetSerialIO(serialObject)

for command in commands:
    sleepTime = 0.1
    if command == 'reboot\n':
        sleepTime = 2.0
    resetSerialIO(serialObject)
    serialObject.write(command)
    time.sleep(sleepTime)
    output = serialObject.read(serialObject.in_waiting)
    resetSerialIO(serialObject)
    print(output, end='')
    time.sleep(sleepTime)

serialObject.close()
