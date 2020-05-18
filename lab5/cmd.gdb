file kernel8.elf
#file sym.elf
target remote :1234
b src/task.c:355
#b src/boot.S:97
#b src/kernel.c:39
continue
