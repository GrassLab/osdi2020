#!/usr/bin/env python3

import serial
import sys

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: ./pyserial.py <path to device>")
        sys.exit()

    try:
        ser = serial.Serial(sys.argv[1], 115200, timeout=5)
    except FileNotFoundError:
        print("Oops! Wrong path of device, please check again.")
        sys.exit()

    print(ser.read_until("> ").decode(), end="")
    ser.timeout = 1;

    size = 0
    while True:
        try:
            size = ser.write((input() + '\n').encode())
            print(ser.read_until("> ").decode()[size:], end="")
        except:
            print("Something went wrong :(")
            break

    ser.close()
