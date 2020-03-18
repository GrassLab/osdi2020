#!/usr/bin/env python3

import serial
import sys

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: ./pyserial.py <path to device>")
        sys.exit()

    try:
        ser = serial.Serial(sys.argv[1], 115200)
    except FileNotFoundError:
        print("Oops! Wrong path of device, please check again.")
        sys.exit()

    size = 0
    while True:
        try:
            print(ser.read_until(b"> ").decode()[size:], end="")
            size = ser.write((input() + '\n').encode())
        except:
            print("Something went wrong :(")
            break

    ser.close()
