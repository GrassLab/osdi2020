#!/bin/bash

if [ $# -gt 0 ]; then
    input="$1"
    while IFS= read -r line
    do
        echo -ne "$line\r" > /dev/ttyUSB0
        sleep 0.1
    done < "$input"
else
    echo "please provide the command list file for this script"
fi
