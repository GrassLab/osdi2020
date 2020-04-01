#!/bin/sh
sudo stty -F /dev/ttyUSB0 115200

sleep 1
echo "help" > /dev/ttyUSB0

sleep 1
echo "hello" > /dev/ttyUSB0

sleep 1
echo "timestamp" > /dev/ttyUSB0

sleep 1
echo "reboot" > /dev/ttyUSB0

