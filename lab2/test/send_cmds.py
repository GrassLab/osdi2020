#!/usr/bin/python3
from serial import Serial
import sys, os
import time

channel = Serial("/dev/ttyUSB0",115200)

if __name__ == '__main__':
	if(len(sys.argv) == 1):
		cmdlist_file = f'{os.path.dirname(__file__)}/cmds.txt'
	else:
		cmdlist_file = sys.argv[1]
	with open(cmdlist_file) as f:
		cmdlist = f.readlines()
	for cmd in cmdlist:
		time.sleep(0.1)	# Wait for exec delay
		channel.write(cmd.encode())
