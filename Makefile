ARMGNU=aarch64-linux-gnu-
CC=$(ARMGNU)gcc
LDFLAGS=-T linker.ld -nostdlib

.PHONY: all clean qemu debug

all: kernel8.img

kernel8.img: boot
	$(ARMGNU)objcopy -O binary boot kernel8.img

clean:
	rm boot kernel8.img

qemu: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -nographic -d in_asm

debug: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -nographic -d in_asm -s -S
