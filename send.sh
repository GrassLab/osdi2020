#!/bin/bash
tty=/dev/ttyUSB0
#tty=/dev/pts/21
size=`stat --printf="%s" $1`
sudo stty -F $tty 115200
printf "load_images"
printf "$size"
printf "524288"
printf sed -E 's/0: (..)(..)(..)(..)/0: \4\3\2\1/' | xxd -r | sudo tee $tty &>/dev/null
sudo tee $tty < $1 &>/dev/null
printf "help"
printf "hello"
