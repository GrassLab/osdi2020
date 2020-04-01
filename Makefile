# NCTU 2020 spring, OSDI Lab 2 by Waylon Shih

CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJC = aarch64-linux-gnu-objcopy
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -Iinc

all: clean kernel8.img

kernel8.img: start.o $(OBJS)
	$(LD) -T linker.ld -o kernel8.elf start.o $(OBJS)
	$(OBJC) -O binary kernel8.elf kernel8.img

start.o: start.S
	$(CC) $(CFLAGS) -c start.S

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f kernel8.elf *.o || true

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio

# Transfer img file to shared folder
teleport:
	cp kernel8.img /media/sf_TELEPORT/kernel8.img
