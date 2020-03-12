import time
from serial import Serial

with open("testfile.txt",'r') as fd:
    with Serial('/dev/ttyUSB0', 115200) as ser:
        line = fd.readline()
        while(line):
            ser.write(line.encode())
            line = fd.readline()
            time.sleep(0.3)