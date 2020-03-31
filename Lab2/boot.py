import serial
import struct
import time
import sys
import os

import tty
import select

def init_Serial(serial_port):
    print("Opening Serial Port ",serial_port)

    ser = serial.Serial(
        port=serial_port,
        baudrate=115200,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS
    )
    return ser

def wait_for_Pi(addr,ser_i):
    print("### Wait until Raspberry Pi is ready......")
   
    try:
        addr = long(addr,0)  
    except ValueError:
        addr = 0x80000
        print("Invalid address for booting, using default", addr)
       
    # write 'c' to trigger 
    ser_i.write('c')
    print('Settiing image on address:',addr)

    address = struct.pack('<l',addr);
    for i in address:
        ser_i.write(i)
    
    cnt = 0
    while cnt < 3:
        x = ser_i.read()
        if x == '\x03':
            cnt = cnt + 1
    
    print("### Raspberry is ready!")

def open_Kernel(kernel_file):

    fid = open(kernel_file, "rb")

    # Get binaries
    data = fid.read()

    # Get file size
    ## (After read, the cursor will point to the end of file, 
    ##  which means the size of the file)
    f_size = fid.tell()
    print("### Kernel image size: %d bytes" % f_size)

    fid.close();
    return f_size, data


def send_Kernel_size(ser_i, size):
    print("\n### Sending Kernel size to RPI")
    
    # send size with little endian
    data = struct.pack('<i',size)

    for i in data:
        ser_i.write(i)

    print("### Waiting for size check on Rasberry Pi......")
    
    # read 2 bytes
    recv = ser_i.read(2)
    
    if recv == "OK":
        print("### Received Acknowledgment!")
    else:
        print("Error after sending size")
        print("Restart")
        return False

    return True



def send_Kernel(ser_i, kernel_data):
    print("### Sending kernel now......")

    start = time.time()
    for tmp, byte in enumerate(kernel_data):
        ser_i.write(byte)
    end = time.time()

    print("Cost time: ", end-start);    
    print("### Finished sending!")

    return True

def start_interactive(ser,input_file,output_file):
        try:
            # set tty to cbreak mode
            tty.setcbreak(input_file.fileno())
            while True:
                rfd, _, _ = select.select([ser, input_file], [], [])

                if ser in rfd:
                    r = ser.read(ser.in_waiting).decode("ascii")
                    output_file.write(r)
                    output_file.flush()

                if input_file in rfd:
                    r = input_file.read(1)
                    ser.write(bytes(r.encode("ascii")))
        except KeyboardInterrupt:
            print("Got keyboard interrupt. Terminating...")
        except OSError as e:
            print("Got OSError. Terminating...")
        finally:
            os.system("stty sane")

def main():
    if (len(sys.argv) == 3): 
        serial_port = sys.argv[1]
        kernel_img = sys.argv[2]

    else:
        print("Arguments wrong! Please check it again!")
        sys.exit()

     
    # Build connectino to target serial port 
    ser_i = init_Serial(serial_port)
    print("### Serial init success!!")

    a = raw_input("### Power on Raspberry Pi and input load address to load kernel img:")
    ## send 'c' to Pi and wait for '\x03\x03\x03' send back
    wait_for_Pi(a,ser_i)

    
    size, kernel_data = open_Kernel(kernel_img)
    guard = send_Kernel_size(ser_i, size)

    if(guard == True):
        send_Kernel(ser_i, kernel_data)
        start_interactive(ser_i,sys.stdin,sys.stdout)
        
if __name__ == "__main__":
    main()
