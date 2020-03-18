all: clean kernel8.img

a.o: a.S
	aarch64-linux-gnu-gcc -c a.S -o a.o

kernel8.img: a.o
	aarch64-linux-gnu-ld a.o -T linker.ld -o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.elf *.o >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm
