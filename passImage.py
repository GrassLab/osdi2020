#!/usr/bin/env python3

import serial
import os
import time
from argparse import ArgumentParser

def passImg(f, d):
    rpi3b = serial.Serial(d, 115200, timeout=0.5) 
    # rpi3b = serial.Serial("/dev/pts/2", 115200, timeout=0.5) 

    file_stats = os.stat(f)
    rpi3b.write("loadimg\n".encode('utf-8'))
    time.sleep(0.01)
    rpi3b.write(str(file_stats.st_size).encode('utf-8'))
    time.sleep(0.01)
    rpi3b.write("\n".encode('utf-8'))
    time.sleep(0.01)
    print('File Size in Bytes is {}'.format(file_stats.st_size))

    with open(f, "rb") as fp:
        byte = fp.read(1)
        while byte:
            rpi3b.write(byte)
            byte = fp.read(1)

            time.sleep(0.001)

    rpi3b.close()

def main():
    parser = ArgumentParser()
    parser.add_argument("-i", help="kernel image of rpi3b", default="./kernel8.img")
    parser.add_argument("-d", help="path to rpi3b device", default="/dev/ttyUSB0")
    args = parser.parse_args()

    passImg(args.i, args.d)

if __name__ == "__main__":
    main()