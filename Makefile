.PHONY: all clean

all: kernel8.img

kernel8.img: start.S
	aarch64-linux-gnu-gcc -c start.S
	aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf start.o
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

start:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm

clean:
	@rm start.o kernel8.*
