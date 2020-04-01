#!/bin/sh
sudo stty -F /dev/ttyUSB0 115200

{
	while :;
	do
	cat /dev/ttyUSB0
	done
}&

sleep 1

echo "now input help"
echo "help" > /dev/ttyUSB0

sleep 1

echo "now input hello"
echo "hello" > /dev/ttyUSB0

sleep 1

echo "now input timestamp"
echo "timestamp" > /dev/ttyUSB0

sleep 1

echo "now input WTF?"
echo "WTF?" > /dev/ttyUSB0

sleep 1

echo "now input reboot"
echo "input" > /dev/ttyUSB0
