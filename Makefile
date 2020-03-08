TOOLCHAIN_PREFIX = aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)ld
OBJCP= $(TOOLCHAIN_PREFIX)objcopy
CFLAGS = -Wall

.PHONY: all clean

all: clean kernel8.img

main.o: main.s
	$(CC) $(CFLAGS) -c main.s -o main.o

kernel8.img: main.o
	$(LD) main.o -T link.ld -o kernel8.elf
	$(OBJCP) -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.elf *.o >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm