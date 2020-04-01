#!/usr/bin/env python3

import os
import serial
import sys
import time

def simple_hash(image):
    checksum = 0
    byte = image.read(1)
    while byte != b"":
        checksum += ord(byte)
        byte = image.read(1)
    return checksum % 65536

def send_image(ser, image):
    byte = image.read(1)
    count = 0
    while byte != b"":
        ser.write(byte)
        count += 1
        print(count)
        byte = image.read(1)
        # Prevent sender from overloading receiver
        time.sleep(0.005)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Usage: ./loadimg.py <IMAGE_PATH> [DEVICE]')
        sys.exit(-1)

    dev = sys.argv[2] if len(sys.argv) > 2 else '/dev/ttyUSB0'
    with serial.Serial(dev, baudrate=115200) as ser:
        ser.timeout = 1

        size = os.path.getsize(sys.argv[1])
        print("Image size: {} bytes".format(size))
        ser.write(size.to_bytes(4, byteorder='little'))

        with open(sys.argv[1], 'rb') as image:
            checksum = simple_hash(image)
            ser.write(checksum.to_bytes(2, byteorder='little'))
            image.seek(0)
            #send_image(ser, image)
            time.sleep(0.05)
            os.system('dd if={} of={}'.format(sys.argv[1], dev))
