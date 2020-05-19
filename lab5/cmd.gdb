file kernel8.elf
#file sym.elf
target remote :1234
#b src/util.S:19
b src/task.c:323
b src/task.c:324
b src/task.c:349
#b src/mm.c:94
#b src/boot.S:97
#b src/kernel.c:39
continue
