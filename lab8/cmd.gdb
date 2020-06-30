file ./build/kernel8.elf
target remote :1234
b tmpfs.c:90
continue
