#!/bin/bash

usage="$(basename "$0") [-h] [-l addr] [-k kernel] [-d device]
program to send the image to indicate device

where:
    -h  show this help message
    -l  set the image location (default: 524288=0x80000)
    -k  indicate the kernel image to upload
    -d  indicate the device to upload
"

# default arguments
loc=$((0x80000))
kernel=kernel8.img
kernel_size=$(wc -c ${kernel} | awk '{print $1}')
dev=/dev/ttyUSB0

while getopts ':hl:k:d:' option; do
  case "$option" in
    h) echo "$usage"
       exit
       ;;
    k) kernel=$OPTARG
       kernel_size=$(wc -c ${kernel} | awk '{print $1}')
       ;;
    l) loc=$(($OPTARG))
       ;;
    d) dev=$OPTARG
       ;;
    :) printf "missing argument for -%s\n" "$OPTARG" >&2
       echo "$usage" >&2
       exit 1
       ;;
   \?) printf "illegal option: -%s\n" "$OPTARG" >&2
       echo "$usage" >&2
       exit 1
       ;;
  esac
done
shift $((OPTIND - 1))

# start sending image
echo "start sending the kernel image
upload $kernel w/t size $kernel_size bytes @ address $(printf '0x%x\n' $loc)
send to device: $dev"

echo -ne "$loc\r" > $dev
echo -ne "$kernel_size\r" > $dev
echo -ne "1\r" > $dev
sleep 0.1
dd status=progress if=$kernel of=$dev # start loading kernel
