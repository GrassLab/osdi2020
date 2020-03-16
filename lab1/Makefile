all: a.o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

kernel8.elf: a.o
	aarch64-linux-gnu-ld -T src/linker.ld -o kernel8.elf a.o

a.o: src/a.S
	aarch64-linux-gnu-gcc -c src/a.S -nostdlib

clean:
	rm *.o *.elf *.img
