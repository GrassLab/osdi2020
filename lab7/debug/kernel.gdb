file kernel8.elf
target remote :1234
#b src/sys.c:140
#b src/fs.c:54
#b src/mm.c:94
#b src/boot.S:97
#b src/mm.c:429
#b src/timer.c:96
#b src/timer.c:96
#b src/task.c:231
#b src/shell.c:86
b src/process.c:196
#b src/process.c:190
#b src/sched.c:84
#b src/mm.c:365
#b src/mm.c:88 if va == 0 && task == 0xffff000000610000
#b src/task.c:145
continue
