#file kernel8.elf
#file sym.elf
file user_build/user.elf
target remote :1234
b user_src/user.c:10
#b src/util.S:19
#b src/mm.c:94
#b src/boot.S:97
#b src/kernel.c:39
continue
