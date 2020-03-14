TOOLCHAIN_PREFIX = aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)ld
OBJCPY = $(TOOLCHAIN_PREFIX)objcopy

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall

.PHONY: all clean asm run debug

all: kernel8.img clean

start.o: start.s
	$(CC) $(CFLAGS) -c start.s -o start.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel8.img: start.o $(OBJS)
	$(LD) start.o $(OBJS) -T linker.ld -o kernel8.elf
	$(OBJCPY) -O binary kernel8.elf kernel8.img

clean:
	rm -f *.o

asm:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial mon:stdio

debug: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s
