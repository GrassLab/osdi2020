# declare variables
CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
QEMU = qemu-system-aarch64

DIR = src

SRCS = $(wildcard $(DIR)/*.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles


.PHONY: clean

all: clean kernel8 

a.o: a.S
	$(CC) $(CFLAGS) -c a.S -o a.o

start.o: start.S
	$(CC) $(CFLAGS) -c start.S -o start.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel8: a.o $(OBJS)
	$(LD) -T linker.ld -o kernel8.elf $< $(OBJS)
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.img kernel8.elf *.o $(DIR)/*.o >/dev/null 2>/dev/null || true

debug: kernel8
	$(QEMU) -M raspi3 -kernel kernel8.img -serial null -serial stdio -S -s

run:
	$(QEMU) -M raspi3 -kernel kernel8.img -serial null -serial stdio