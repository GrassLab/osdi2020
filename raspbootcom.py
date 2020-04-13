#!/usr/bin/python3
import serial
import os
import sys
import logging, coloredlogs
import termios

'''
create logger
'''
coloredlogs.install()
logger = logging.getLogger(__file__)
logger.setLevel(logging.DEBUG)
'''
get kernel image size
'''
KERNEL_IMG_PATH = "/media/sf_Documents/OSDI/osdi2020/kernel8.img"
#KERNEL_IMG_PATH = "/media/sf_Documents/OSDI/raspi3-tutorial/05_uart0/kernel8.img"
KERNEL_IMG_SIZE = os.stat(KERNEL_IMG_PATH).st_size

def send_size(s, kernel_size):

    print(s.readline()) ### b"Please send the kernel size...\r\n"
    s.write(bytes( [kernel_size & 0xff] ))
    s.write(bytes( [(kernel_size >> 8) & 0xff] ))
    s.write(bytes( [(kernel_size >> 16) & 0xff] ))
    s.write(bytes( [(kernel_size >> 24) & 0xff] ))

    raspi3_recv = b""
    for _ in range(2):
        raspi3_recv += s.read()
    print(raspi3_recv)
    if (raspi3_recv != b"OK"):
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

    
    while True:
        NO_REBOOT = True
        
        while NO_REBOOT:
            
            res_line = b''
            while True:
                res = s.read()
                res_line += res
                #print(f'read: {res}')
                if (res_line == b'>'):
                    break 
                if (res == b'\n'):
                    print(res_line)
                    sys.stdout.flush()
                    
                    if b'HANK0438\r\n' in res_line:
                        logger.info("rebooting...")
                        NO_REBOOT = False
                        break

                    res_line = b''

            if NO_REBOOT:
                #termios.tcflush(sys.stdin, termios.TCIOFLUSH)
                command = input(">").strip()
                logger.info(f"cmd > {command}")
                s.write(command.encode()+b'\n')

        logger.info("sending size...")
        if send_size(s, KERNEL_IMG_SIZE):
            print(s.readline()) ### b"Please input the kernel load address (default: 0x80000):\r\n"
            print(s.readline()) ### b"Please send the kernel from UART...\n"
            logger.info("sending kernel...")
            send_kernel(s, KERNEL_IMG_SIZE)
            logger.info("finish!")
            print(s.readline()) ### b'Loading kernel at 0x00100000 with size 0x00007010 ...\r\n'
main()