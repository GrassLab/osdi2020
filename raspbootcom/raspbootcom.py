#!/usr/bin/python3
import serial
import os
import sys
import logging, coloredlogs

'''
create logger
'''
coloredlogs.install()
logger = logging.getLogger("raspbootcom")
logger.setLevel(logging.DEBUG)
'''
get kernel image size
'''
KERNEL_IMG_PATH = "/media/sf_Documents/OSDI/osdi2020/kernel8.img"
KERNEL_IMG_SIZE = os.stat(KERNEL_IMG_PATH).st_size

def init_connection(s):
    raspi3_recv1 = b""
    raspi3_recv2 = b""

    for _ in range(3):
        raspi3_recv1 += s.read()
    print(raspi3_recv1)
    if (raspi3_recv1 != b"\x03\x03\x03"):
        return False

    s.write(bytes( [KERNEL_IMG_SIZE & 0xff] ))
    s.write(bytes( [(KERNEL_IMG_SIZE >> 8) & 0xff] ))
    s.write(bytes( [(KERNEL_IMG_SIZE >> 16) & 0xff] ))
    s.write(bytes( [(KERNEL_IMG_SIZE >> 24) & 0xff] ))

    for _ in range(2):
        raspi3_recv2 += s.read()
    print(raspi3_recv2)
    if (raspi3_recv2 != b"OK"):
        return False
    
    return True

def send_kernel(s, kernel_size):
    f = open(KERNEL_IMG_PATH, "rb")
    while (kernel_size>0):
        s.write(f.read())
        kernel_size -= 1


def main():
    s = serial.Serial("/dev/ttyUSB0", 115200)

    logger.info(f'kernel size: {KERNEL_IMG_SIZE}')
    logger.info("waiting...")

    print(s.readline()) ### b'RBIN64\r\n'
    while True:
        NO_REBOOT = True
        logger.info("initialize connection...")
        if init_connection(s):
            logger.info("sending kernel...")
            send_kernel(s, KERNEL_IMG_SIZE)
            logger.info("finish!")

            for _ in range(3):
                res = s.readline().decode()
                res = res.strip()
                print(res)
            
            s.read() ### b'>'
            while NO_REBOOT:
                #logger.info("cmd > reboot")
                #s.write(b'reboot\n')
                command = input(">").strip()
                logger.info(f"cmd > {command}")
                s.write(command.encode()+b'\n')
                
                res_line = b''
                while True:
                    res = s.read()
                    res_line += res
                    #print(f'read: {res}')
                    if (res_line == b'>'):
                        break 
                    if (res == b'\n'):
                        print(res_line)
                        
                        if b'RBIN64\r\n' in res_line:
                            logger.info("rebooting...")
                            NO_REBOOT = False
                            break

                        res_line = b''   
main()