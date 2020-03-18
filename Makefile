kernel8.img: kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img
kernel8.elf: start.o main.o uart.o print_figlet.o
	aarch64-linux-gnu-ld -T linker_new.ld -o kernel8.elf start.o main.o uart.o print_figlet.o
start.o: start.S
	aarch64-linux-gnu-gcc -nostdlib -c start.S
main.o: main.c uart.c uart.h
	aarch64-linux-gnu-gcc -nostdlib -c main.c
uart.o: uart.c uart.h
	aarch64-linux-gnu-gcc -nostdlib -c uart.c
print_figlet.o: print_figlet.c uart.h
	aarch64-linux-gnu-gcc -nostdlib -c print_figlet.c
clean:
	rm *.o kernel8.*
