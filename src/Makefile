CC	:= aarch64-linux-gnu-gcc
LD	:= aarch64-linux-gnu-ld

.PHONY: all test clean
all: test

a.o: a.S
	aarch64-linux-gnu-gcc -c $<

kernel8.elf: a.o
	aarch64-linux-gnu-ld -T linker.ld -o $@ $<

kernel8.img: kernel8.elf
	aarch64-linux-gnu-objcopy -O binary $< $@

test: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel $< -display none -d in_asm

clean:
	rm -rf a.o kernel8.*
