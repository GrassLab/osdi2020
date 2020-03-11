# declare variables
CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
QEMU = qemu-system-aarch64

.PHONY: clean

all: a.S
	$(CC) -c a.S
	$(LD) -T linker.ld -o kernel8.elf a.o
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.img kernel8.elf *.o >/dev/null 2>/dev/null

run:
	$(QEMU) -M raspi3 -kernel kernel8.img -display none -d in_asm