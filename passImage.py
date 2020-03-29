#!/usr/bin/env python3

import serial
import os
from argparse import ArgumentParser

def passImg(f):
    # rpi3b = serial.Serial("/dev/ttyUSB0", 115200, timeout=0.5) 
    rpi3b = serial.Serial("/dev/pts/23", 115200, timeout=0.5) 

    file_stats = os.stat(f)
    rpi3b.write("loadimg\n".encode('utf-8'))
    rpi3b.write(str(file_stats.st_size).encode('utf-8'))
    rpi3b.write("\n".encode('utf-8'))
    print(f'File Size in Bytes is {file_stats.st_size}')

    with open(f, "rb") as fp:
        byte = fp.read(1)
        while byte:
            rpi3b.write(byte)
            byte = fp.read(1)
    
    rpi3b.close()

def main():
    parser = ArgumentParser()
    parser.add_argument("-i", help="kernel image of rpi3b", default="./kernel8.img")
    args = parser.parse_args()

    passImg(args.i)

if __name__ == "__main__":
    main()