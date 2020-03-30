#!/bin/bash
size=`stat --printf="%s" $1`
sudo stty -F /dev/pts/0 115200
printf "0: %.8x" $size | sed -E 's/0: (..)(..)(..)(..)/0: \4\3\2\1/' | xxd -r | sudo tee /dev/pts/0 &>/dev/null
sudo tee /dev/pts/0 < $1 &>/dev/null
