#file kernel8.elf
file sym.elf
target remote :1234
b src/boot.S:49
b src/boot.S:172
b src/boot.S:324
b src/boot.S:335
#b src/boot.S:97
b src/kernel.c:29
#b src/kernel.c:39
continue
