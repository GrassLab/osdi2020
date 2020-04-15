#!/bin/sh
aarch64-linux-gnu-gdb -ex 'file kernel8.elf' -ex 'target remote :1234' -ex 'display/10i $pc' -ex 'b main'
