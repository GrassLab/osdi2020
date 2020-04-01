import string
import serial
import time
import os
from array import array
        
def compute_kernel_checksum(kernel_bytes):
    num = 0
    for b in kernel_bytes:
        num = (num + b) % (2 ** 32)
    return num

PORT = '/dev/pts/3'
# PORT = '/dev/ttyUSB0'
BAUD_RATES = 115200

kernel_path = './kernel/kernel83.img'
kernel_size = os.path.getsize(kernel_path)

ser = serial.Serial(PORT, BAUD_RATES)

ser.flush()
ser.flushInput()
ser.flushOutput()

# 0x80000 = 524288
# content = ["loadimage\n", str(kernel_size)+"\n", "524288\n"]
content = ["loadimage\n", str(kernel_size)+"\n"]

try:   
    with open(kernel_path, "rb") as f:
        line = "loadimage\n"
        print(line)
        ser.write(content[0].encode())        
        ser.flush()
        ser.flushInput()
        ser.flushOutput()
        time.sleep(1)
        
        kernel = f.read()
        size = len(kernel)
        checksum = compute_kernel_checksum(kernel)
        print("Sending kernel with size", size, "and checksum", checksum)
        # ser.write(size.to_bytes(4, byteorder='big'))
        ser.write(content[1].encode())
        ser.flush()
        ser.flushInput()
        ser.flushOutput()
        time.sleep(1)

        # number_bytes = ser.read(4)
        # size_confirmation =  int.from_bytes(number_bytes, byteorder='big')
        # if size_confirmation != size:
        #     print("Expected size to be", size, "but got", size_confirmation)
        # else:
        #     print("Size OK!")
        ser.write(kernel)
        ser.flush()
        ser.flushInput()
        ser.flushOutput()
        time.sleep(1)


    # print(1)
    # bytes_to_read = 4
    # number_bytes = ser.read(bytes_to_read)
    # print(2)
    # checksum_confirmation = int.from_bytes(number_bytes, byteorder='big')

    # if checksum_confirmation != checksum:
    #     print("Expected checksum to be", checksum, "but was", checksum_confirmation)
    # else:
    #     print("Checksum OK!")
    time.sleep(3)

    ser.flush()
    ser.flushInput()
    ser.flushOutput()

finally:
    print("end")
    ser.close()