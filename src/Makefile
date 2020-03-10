CC	    := aarch64-linux-gnu-gcc
LD	    := aarch64-linux-gnu-ld
OBJCOPY := aarch64-linux-gnu-objcopy

.PHONY: all test clean
all: test

a.o: a.S
	$(CC) -c $<

kernel8.elf: a.o
	$(LD) -T linker.ld -o $@ $<

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary $< $@

test: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel $< -display none -d in_asm

clean:
	rm -rf a.o kernel8.*
