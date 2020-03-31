import os
import serial
import argparse
filename = "nctuos.img"

parser = argparse.ArgumentParser()

parser.add_argument("input", help="the input file")
parser.add_argument("tty", help="the target tty")
parser.add_argument("-b", "--baudrate",
                    help="serial baudrate, the default value is 115200", default=115200)

args = parser.parse_args()

filename = args.input
port = args.tty
baudrate = args.baudrate



def bytes_from_file(filename, chunksize=8192):
    with open(filename, "rb") as f:
        chunk = f.read(chunksize)
        return chunk

with serial.Serial(port, baudrate) as ser:
    size = os.path.getsize(filename)
    print(f'Image size is {size}')
    res = f"{size}\n"
    ser.write(res.encode())
    input("ready?")
    img = bytes_from_file(filename)
    ser.write(img)
    print("done")

