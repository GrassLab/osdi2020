import serial
import argparse
from time import sleep

parser = argparse.ArgumentParser()

parser.add_argument("input", help="the input file")
parser.add_argument("tty", help="the target tty")
parser.add_argument("-b", "--baudrate", help="serial baudrate, the default value is 115200", default=115200)

args = parser.parse_args()

filename = args.input
port = args.tty
baudrate = args.baudrate

with open(filename) as f:
    lines = f.readlines()
    ser = serial.Serial(port, baudrate)
    for line in lines:
        ser.write(line.encode())
        sleep(0.05)
    ser.close()
