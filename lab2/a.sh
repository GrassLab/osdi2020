#/bin/bash

cat test.txt | while read line 
do
    echo $line > /dev/ttyUSB0
    sleep 1
done

