kernel8.img: kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img
kernel8.elf: start.o main.o my_shell.o uart.o mailbox.o exception.o core_timer.o
	aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf start.o main.o my_shell.o uart.o mailbox.o exception.o core_timer.o
main.o: main.c uart.h exception.h my_shell.h
	aarch64-linux-gnu-gcc -nostdlib -c main.c
my_shell.o: my_shell.h my_shell.c mailbox.h exception.h uart.h core_timer.S
	aarch64-linux-gnu-gcc -nostdlib -c my_shell.c
mailbox.o: mailbox.h mailbox.c uart.h
	aarch64-linux-gnu-gcc -nostdlib -c mailbox.c
exception.o: uart.h exception.h exception.c core_timer.S start.S
	aarch64-linux-gnu-gcc -nostdlib -c exception.c
start.o: start.S exception.h
	aarch64-linux-gnu-gcc -nostdlib -c start.S
uart.o: uart.c uart.h
	aarch64-linux-gnu-gcc -nostdlib -c uart.c
core_timer.o: core_timer.S my_shell.h 
	aarch64-linux-gnu-gcc -nostdlib -c core_timer.S
clean:
	rm *.o kernel8.*
