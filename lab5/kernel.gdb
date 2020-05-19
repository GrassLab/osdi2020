file kernel8.elf
target remote :1234
#b src/sys.c:36
#b src/mm.c:94
#b src/boot.S:97
#b src/kernel.c:39
b src/task.c:337
continue
