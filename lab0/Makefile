all: a.o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

kernel8.elf: a.o
	aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf a.o

a.o: a.S
	aarch64-linux-gnu-gcc -c a.S
clean: 
	rm *.o *.elf *.img
