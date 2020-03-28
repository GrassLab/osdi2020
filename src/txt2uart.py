import time
import argparse
from serial import Serial

parser = argparse.ArgumentParser(description='NCTU.OSDI.Lab course')
parser.add_argument('--filename', metavar='PATH', default='./testfile.txt', type=str, help='path to script')
parser.add_argument('--device', metavar='TTY',default='/dev/ttyUSB0', type=str,  help='path to UART device')
parser.add_argument('--baud', metavar='Hz',default=115200, type=int,  help='baud rate')
parser.add_argument('--delay', metavar='s',default=0.3, type=float,  help='delay between scripts')
args = parser.parse_args()

with open(args.filename,'r') as fd:
    with Serial(args.device, 115200) as ser:
        line = fd.readline()
        while(line):
            ser.write(line.encode())
            line = fd.readline()
            time.sleep(0.3)