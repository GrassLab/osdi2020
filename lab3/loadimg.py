#!/usr/bin/env python3

import serial
import sys
import os

def sendImage(ser, image_path):
    print(ser.read_until(b"> ").decode(), end="")
    ser.write("loadimg\n".encode())
    print(ser.read_until(b"Please input kernel image size (max: 262144): ").decode(), end="")

    try:
        size = os.path.getsize(image_path)
        ser.write((str(size) + "\n").encode())
    except:
        print("Something went wrong when trying to send image size")
        ser.close()
        sys.exit()

    response = ser.read_until(b"Start loading os kernel image ...\n").decode()
    if "Start" not in response:
        print("Oops something went wrong. Got message: ", end="")
        print(response)
        ser.close()
        sys.exit()
    print(response, end="")

    with open(image_path, "rb") as image:
        while size > 0:
            size -= ser.write(image.read(0x400))


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: ./loadimg.py <path to device> <path to image>")
        sys.exit()

    try:
        ser = serial.Serial(sys.argv[1], 115200, timeout=5)
    except FileNotFoundError:
        print("Oops! Wrong path of device, please check again.")
        sys.exit()

    sendImage(ser, sys.argv[2])

    size = 0
    while True:
        try:
            print(ser.read_until(b"> ").decode()[size:], end="")
            size = ser.write((input() + '\n').encode())
        except:
            print("Something went wrong :(")
            break

    ser.close()
