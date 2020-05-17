#file kernel8.elf
file sym.elf
target remote :1234
b src/boot.S:129
b src/kernel.c:10
b src/kernel.c:39
continue
