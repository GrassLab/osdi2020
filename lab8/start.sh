#!/bin/bash

echo "###"
echo "### Use C-a h for help"
echo "###"
echo ""

qemu-system-aarch64 -machine raspi3 -nographic -drive if=sd,file=./sfn_nctuos.img,format=raw -kernel ./kernel8.img -serial null -serial mon:stdio
