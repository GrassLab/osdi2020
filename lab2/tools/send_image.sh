#!/bin//bash

KER=${1}
KER_SZ=$(wc -c ${1} | awk '{print $1}')
DEV=${2}

# echo -ne "524288\r" > $DEV # start @ loc
echo -ne "0\r" > $DEV
sleep 0.2
echo -ne "$KER_SZ\r" > $DEV
sleep 0.2
echo -ne "1\r" > $DEV
sleep 0.2
dd if=$KER of=$DEV # start loading kernel
