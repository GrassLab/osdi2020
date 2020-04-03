kernel8.img: kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img
kernel8.elf: start.o main.o uart.o print_figlet.o mailbox.o
	aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf start.o main.o uart.o print_figlet.o mailbox.o
start.o: start.S
	aarch64-linux-gnu-gcc -nostdlib -c start.S
main.o: main.c uart.c uart.h mailbox.c mailbox.h
	aarch64-linux-gnu-gcc -nostdlib -c main.c
mailbox.o: mailbox.h uart.h
	aarch64-linux-gnu-gcc -nostdlib -c mailbox.c
uart.o: uart.c uart.h
	aarch64-linux-gnu-gcc -nostdlib -c uart.c
print_figlet.o: print_figlet.c uart.h
	aarch64-linux-gnu-gcc -nostdlib -c print_figlet.c
clean:
	rm *.o kernel8.*
