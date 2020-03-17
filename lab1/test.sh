#!/bin/bash

sudo bash -c 'echo -ne "help\r" > /dev/ttyUSB0'
sleep 1
sudo bash -c 'echo -ne "timestamp\r" > /dev/ttyUSB0'
sleep 1
sudo bash -c 'echo -ne "hello\r" > /dev/ttyUSB0'
sleep 1
sudo bash -c 'echo -ne "reboot\r" > /dev/ttyUSB0'
sleep 1
