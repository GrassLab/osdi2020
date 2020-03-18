all: kernel8.img

start.o: start.S
	aarch64-linux-gnu-gcc -c start.S -o start.o

main.o: main.c
	aarch64-linux-gnu-gcc -c main.c -o main.o

uart.o: uart.c
	aarch64-linux-gnu-gcc -c uart.c -o uart.o

kernel8.elf: start.o uart.o main.o
	aarch64-linux-gnu-ld -T link.ld -o kernel8.elf start.o uart.o main.o

kernel8.img: kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

clean: 
	rm kernel8.elf *.o ||true

run:	
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio
