import os
import sys
import serial
import struct
from itertools import zip_longest

def shell(ser):
    while True:
        cmd = input() + '\n'
        ser.write(cmd.encode())
        if cmd == 'loadimg\n':
            transfer_kernel(ser)
        else:
            print('waiting response')
            data = ser.read_until(terminator=b'# ')
            print(data.decode(), end='')


def reorder(gg):
    g2 = gg.decode('ascii')
    d = list()
    for i in range(32):
        for j in range(4):
            d.append(i*4 + 3-j)
    return d.encode()


def transfer_kernel(ser):
    ser.read_until(b'loadimg\n')
    size_ = get_kernel_size('kernel8.img')
    ser.read_until(b'ok')
    print('kernel copy ... ok')
    ser.write(size_)
    data = ser.read_until()
    assert int(data.decode()) == int(size_.decode())
    print(f'size {size_} check ... ok')
    data = read_kernel('kernel8.img')
    chunksize = 128
    cnt = 0
    while cnt*chunksize < int(size_):
        chunkdata = data[cnt*chunksize:(cnt+1)*chunksize]
        print(chunkdata[:10])
        # assert(len(gchunkdata) == len(chunkdata))
        # print(chunkdata)
        ser.write(str(len(chunkdata)).encode())
        ret = ser.read_until(b'ready')
        print(ret)
        print(f'dbg: pi is ready to receive {len(chunkdata)} size data')
        ser.write(chunkdata)
        print('dbg: write done, wait response')
        ret = ser.read_until(b'done')
        cnt += 1
        print(ret)
    ret = ser.read_until(b'okload')
    print(ret)
    print('kernel upload ... ok')
    

def chunkize(data, chunksize):
    args = [iter(data)] * chunksize
    return [b''.join(t) for t in zip_longest(*args, fillvalue=b'')]


def get_kernel_size(filename):
    size_ = os.stat(filename).st_size
    return (str(size_) + '\n').encode()


def read_kernel(filename):
    data = list()
    with open(filename, 'rb') as file:
        while True:
            byte = file.read(1)
            if not byte:
                break
            else:
                data.append(byte)
    return b''.join(data)
    

if __name__ == '__main__':
    ser = serial.Serial(sys.argv[1], baudrate=115200,
                     parity=serial.PARITY_NONE,
                     stopbits=serial.STOPBITS_ONE,
                     bytesize=serial.EIGHTBITS)
    shell(ser)
    # data = read_kernel('kernel8.img')
    # a = chunkize(data, 128)
