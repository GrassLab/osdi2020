import os
import serial
import argparse
filename = "nctuos.img"

parser = argparse.ArgumentParser()

parser.add_argument("input", help="the input file")
parser.add_argument("tty", help="the target tty")
parser.add_argument("ttyin", help="the target tty in")
parser.add_argument("-b", "--baudrate",
                    help="serial baudrate, the default value is 115200", default=115200)

args = parser.parse_args()

filename = args.input
port = args.tty
baudrate = args.baudrate
port_in = args.ttyin



def bytes_from_file(filename, chunksize=8192):
    with open(filename, "rb") as f:
        chunk = f.read(chunksize)
        return chunk

with serial.Serial(port_in, baudrate) as ser_in:
    with serial.Serial(port, baudrate) as ser:
        while True:
            while ser_in.in_waiting > 0:
                print("b")
                tmp = ser_in.read()
                ser.write(tmp)
            while ser.in_waiting > 0:
                print("a")
                tmp = ser.read()
                print(tmp)
                if tmp == b'\x11':
                    size = os.path.getsize(filename)
                    print(f'Image size is {size}')
                    ser.read_until(b"Image size: ")
                    ser_in.write(b"Image size: ");
                    res = f"{size}\r"
                    ser.write(res.encode())
                    img = bytes_from_file(filename)
                    ser.write(img)
                    print("done")
                else:
                    ser_in.write(tmp);


