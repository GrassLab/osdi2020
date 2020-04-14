# NCTU 2020 spring, OSDI Lab 2 by Waylon Shih

CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
COBJ = aarch64-linux-gnu-objcopy
SRCS = $(wildcard src/*.c)
ASMS = $(wildcard src/*.S)
OBJS = $(SRCS:.c=.o) $(ASMS:=.o)
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -Iinc

all: clean kernel8.img

kernel8.img: $(OBJS)
	$(LD) -T linker.ld -o kernel8.elf $(OBJS)
	$(COBJ) -O binary kernel8.elf kernel8.img

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.S.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f kernel8.elf src/*.o || true

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio

# Transfer img file to shared folder
teleport:
	cp kernel8.img /media/sf_TELEPORT/kernel8.img
