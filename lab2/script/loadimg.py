# coding=utf-8

import serial
import time
import os

KERNEL_PATH = './kernel8.img'

def serial_w(content):
    ser.write(content)
    time.sleep(1)

if __name__ == "__main__":
    ser = serial.Serial(port='/dev/pts/21', baudrate=115200)
    kernel_size = os.path.getsize(KERNEL_PATH)
    with open(KERNEL_PATH, 'rb') as kernel_f:

        # cmd
        serial_w('loadimg\r')
        # addr
        serial_w('0\r')
        
        words = kernel_f.read()
        # kernel size
        serial_w(str(len(words))+'\r')

        # serial_w(words)
        print('hi')
        kernel_f.close()
