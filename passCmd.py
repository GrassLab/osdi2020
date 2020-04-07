#!/usr/bin/env python3

import serial
from argparse import ArgumentParser

def passCmd(f):
    # rpi3b = serial.Serial("/dev/ttyUSB0", 115200, timeout=0.5) 
    rpi3b = serial.Serial("/dev/pts/23", 115200, timeout=0.5) 

    with open(f, 'r') as fp:
        line = fp.readline()
 
        while line:
            print(line)
            rpi3b.write(line.replace('\n', '\r').encode('utf-8'))
            line = fp.readline()
    
    rpi3b.close()

def main():
    parser = ArgumentParser()
    parser.add_argument("-f", help="text file that contains rpi3b commands", default="./cmd.txt")
    args = parser.parse_args()

    passCmd(args.f)

if __name__ == "__main__":
    main()