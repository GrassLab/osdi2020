import serial
import argparse
import time

parser = argparse.ArgumentParser(description='a python3 script that can read command from file and interact with raspi3')
parser.add_argument('-file_path', '--file_path', type=str, help='the input file path')
args = parser.parse_args()

ser=serial.Serial("/dev/ttyUSB0",115200,timeout=0.5) 

f = open(args.file_path,'r')
for line in f.readlines():
    print(line)
    ser.write(str.encode(line))
    time.sleep(0.2)
ser.close()