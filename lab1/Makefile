all: kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

kernel8.elf: object
	aarch64-linux-gnu-ld start.o uart.o main.o -T src/linker.ld -o kernel8.elf

object: src/start.S src/uart.c src/main.c
	aarch64-linux-gnu-gcc -c src/start.S -nostdlib
	aarch64-linux-gnu-gcc -c src/uart.c -nostdlib
	aarch64-linux-gnu-gcc -c src/main.c -nostdlib

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio

clean:
	rm *.o *.elf *.img
