#!/usr/bin/env python3

import serial
import sys
import time

if __name__ == '__main__':

    # Open serial
    ser = serial.Serial('/dev/cu.SLAB_USBtoUART', 115200, timeout=1)
    ser.write("\n".encode())
    print(ser.read_until("# ").decode(), end="")
    '''
    ser.timeout=None
    for i in range(10):
        print(ser.readline().decode(), end="")
    
    #print(ser.read_until("# ").decode(), end="")
    '''
    # set the serial to blocking mode
    ser.timeout=None
    tdata = ser.read()
    # wait for the device to waiting data to bus
    time.sleep(1)
    # calculate the current size of input data on the bus
    data_left = ser.inWaiting()
    # read all the data on the bus
    tdata = tdata + ser.read(data_left)
    # print the boot message on the screen
    print(tdata.decode(), end="")
    # set the serial to non blocking mode
    ser.timeout=1
    # input loop
    while True:
        line_buffer = input()
        # encode input command and write the command on the bus
        ser.write((line_buffer.strip() + '\r').encode())
        if line_buffer == "exit":
            break
        elif line_buffer == "reboot":   # one more read to pass the waiting(IDK WHY)
            print(ser.readline().decode(), end="")
        elif line_buffer == "loadimg":  # send the kernel image to rpi3
            pass
        
        # read the input data until the pound sign comes up
        print(ser.read_until("# ").decode(), end="")

    ser.close()
