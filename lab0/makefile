all:
	aarch64-linux-gnu-gcc -c a.S
	aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf a.o
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img
run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm
