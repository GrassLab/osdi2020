import serial
from time import sleep


f = open("cmd.txt","r")
line = f.readline()

##
COM_PORT = '/dev/ttyUSB0'

BAUD_RATES = 115200
ser = serial.Serial(COM_PORT,BAUD_RATES)

try:
    while line:
        print('Send cmd:',line)
        ser.write(line[:-1]+str('\r'))
        

        while ser.in_waiting:
            feedback = ser.readline().decode()
            print('Feedback:',feedback)

        line = f.readline()

except KeyboardInterrupt:
    ser.close()
    print('close')

f.close()
