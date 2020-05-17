file kernel8.elf
#file sym.elf
target remote :1234
b src/boot.S:48
b src/boot.S:162
#b src/boot.S:97
b src/kernel.c:29
#b src/kernel.c:39
continue
