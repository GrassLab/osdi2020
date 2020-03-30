import argparse
import serial
import os
import numpy as np

parser = argparse.ArgumentParser()
parser.add_argument("image")
parser.add_argument("tty")
args = parser.parse_args()


def checksum(bytecodes):
    # convert bytes to int
    return int(np.array(list(bytecodes), dtype=np.int32).sum())


def main():
    try:
        ser = serial.Serial(args.tty, 115200)
    except:
        print("Serial init failed!")
        exit(1)

    file_path = args.image
    file_size = os.stat(file_path).st_size

    with open(file_path, 'rb') as f:
        bytecodes = f.read()

    file_checksum = checksum(bytecodes)

    ser.write(file_size.to_bytes(4, byteorder="big"))
    ser.write(file_checksum.to_bytes(4, byteorder="big"))
    ser.write(bytecodes)

    print(f"Image Size: {file_size}, Checksum: {file_checksum}")


if __name__ == "__main__":
    main()
