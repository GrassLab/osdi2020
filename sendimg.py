#!/usr/bin/env python3

import serial
import sys
import time
import os
import struct

def print_bytecode_in_hex(bytecode):
    print(":".join('{0:#0{1}x}'.format(c, 4) for c in bytecode))

def sumhash(bytecode):
    return sum([int(byte) for byte in bytecode])


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print(f"{sys.argv[0]} <img_name> <tty>")
        exit()

    filesize = os.path.getsize(sys.argv[1])
    input_file = open(sys.argv[1], 'rb')

    # while True:
        # chunk_0x100 = input_file.read(0x100)
        # print(sumhash(chunk_0x100))
        # exit()

    # Open serial
    if sys.argv[2].find('pts') != -1:
        term = open(sys.argv[2], 'wb+', buffering=0)
    else:
        term = serial.Serial(sys.argv[2], 115200, timeout=1)


    # Check if in interactive mode
    term.write("\n".encode())

    if term.read(1).decode() == '\n':
        print(term.read(len('# ')))
        term.write("loadimg\n".encode())
        time.sleep(0.1)
        print(term.read(len("loadimg\n")))
        print(term.read(len("Start loading os kernel image\n")))
        print(term.read(len("Kernel .text offset (default 0x80000): ")))
        if len(sys.argv) == 4:
            term.write(f'{sys.argv[3]}\n'.encode())
            time.sleep(0.1)
            print(term.read(len(f'{sys.argv[3]}\n')))
        else:
            term.write('\n'.encode())
            time.sleep(0.1)
            print(term.read(len('\n')))
        print(term.read(len("Send image now using sendimg.py\n")))
        term.write("\x00H3lloloadimg\x00".encode())
        time.sleep(0.1)
    elif term.read(len("3llosendimg")):
        term.write("\x00H3lloloadimg\x00".encode())
        time.sleep(0.1)
        print(term.read(4).decode())
    else:
        raise Exception
        exit()

    term.write(struct.pack("<I", filesize))

    print(f"Image size {filesize}")
    print("Sending image chunk ...", end="")
    current_chunk = 0
    while True:
        chunk_0x100 = input_file.read(0x100)
        term.write(chunk_0x100)
        term.write(struct.pack("<H", sumhash(chunk_0x100))) # 2 byte sumhash
        # print(sumhash(chunk_0x100))
        # print(len(chunk_0x100))
        # print_bytecode_in_hex(chunk_0x100)
        time.sleep(0.1)
        resp = term.read(2).decode()
        if resp == "FI":
            current_chunk += 1
            print(f" {current_chunk} ...", end="", flush=True)
        elif resp == "BY":
            print("Complete")
            exit()
        elif resp == "DE":
            term.read(len("Failed to receive image\n"))
            print("Loadimg report chunk corrupted")
            term.close()
            exit()
        else:
            print(f"Last received: {resp}")
            raise Exception

