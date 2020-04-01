import serial
import sys
import time
import struct


with open("kernel8.img","rb") as f:
	data=f.read()
	ser=serial.Serial("/dev/ttyUSB0",115200,timeout=0.5)
	size=f.tell()
	print(size)
	t=time.time()
	ser.write(b'loadimg\n')
	ser.flush()
	print(ser.readline().decode())
	if(ser.readline()==b'\rrec\n'):
		size_b=struct.pack('<i',size)
		ser.flush()
		for b in size_b:
			ser.write(b)
			print(b)
		if(ser.readline()==b'\rOK\n'):		
			for num,byte in enumerate(data):
				if(num%100==0):
					print(num,"/",size)
				ser.write(byte)
			ser.flush()
			print(ser.readline())
			print(time.time()-t)
		else:
			print("didn't receive size")
	else:	
		print("ERROR!")	
			
