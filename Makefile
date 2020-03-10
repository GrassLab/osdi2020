TOOLCHAIN_PREFIX = aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)ld
OBJCP= $(TOOLCHAIN_PREFIX)objcopy
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -g -nostdlib -nostartfiles

.PHONY: all clean run debug

all: clean kernel8.img

start.o: start.S
	$(CC) $(CFLAGS) -c start.S -o start.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel8.img: start.o $(OBJS)
	$(LD) start.o $(OBJS) -T link.ld -o kernel8.elf
	$(OBJCP) -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.img kernel8.elf *.o >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio 

debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -s -S