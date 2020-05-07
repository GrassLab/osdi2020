#!/usr/bin/python3
from serial import Serial
import sys, os
import time

channel = Serial("/dev/ttyUSB0",115200)

if __name__ == '__main__':
	kernel_image_path = './kernel8.img'
	with open(kernel_image_path, "rb") as f:
		image = f.read(1)
		while(True):
			c = f.read(1)
			if not c:
				break
			image += c
	print(len(image))
	channel.write(len(image).to_bytes(4, 'little') + image)
	# channel.write(len(image).to_bytes(4, 'little'))
	for byte in len(image).to_bytes(4, 'little'):
	# 	channel.write(byte)
		print(byte)
	# for byte in image:
	# 	channel.write(byte)
	# 	print(byte)
