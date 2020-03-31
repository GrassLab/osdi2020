#!/bin//bash

KER=${1}
KER_SZ=$(wc -c ${1} | awk '{print $1}')
DEV=${2}

echo -ne "S@458752\r" > $DEV # start @ loc
echo -ne "$KER_SZ\r" > $DEV
dd if=$KER bs=64 of=$DEV # start loading kernel
echo -ne "E" > $DEV # end
