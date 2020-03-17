#!/usr/bin/env python3

import serial
import sys

if __name__ == '__main__':

    # Open serial
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
    ser.write("\n".encode())
    print(ser.read_until("# ").decode(), end="")

    input_file = open(sys.argv[1], 'r')
    for line in input_file:
        ser.write((line.strip() + '\r').encode())
        print(ser.read_until("# ").decode(), end="")

    ser.close()
