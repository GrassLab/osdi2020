# coding=utf-8

import serial
import time
import os

KERNEL_PATH = './kernel9.img'

def serial_w(content):
    ser.write(content)
    time.sleep(1)

port1 = '/dev/pts/4'
port2 = '/dev/ttyUSB0'

if __name__ == "__main__":
    ser = serial.Serial(port=port1, baudrate=115200)
    kernel_size = os.path.getsize(KERNEL_PATH)
    with open(KERNEL_PATH, 'rb') as kernel_f:

        # cmd
        serial_w('loadimg\r')
        # addr
        serial_w('90000\r')
        
        
        # kernel size
        k_size = os.path.getsize(KERNEL_PATH)
        serial_w(str(k_size)+'\r')

        # while k_size > 0:
        #     words = kernel_f.read(0x400)
        #     k_size -= ser.write(words)
        #     # k_size -= words

        
        words = kernel_f.read()
        serial_w(words)
        serial_w('F\r')
        print('hi')
        kernel_f.close()
