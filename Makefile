SHELL = /bin/sh
ARM = aarch64-linux-gnu
CC = $(ARM)-gcc
LD = $(ARM)-ld
OBJCOPY = $(ARM)-objcopy
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: clean kernel8.img

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

kernel8.elf: start.o linker.ld $(OBJS)
	$(LD) -T linker.ld -o kernel8.elf start.o $(OBJS)

%.o: %.c
	$(CC) -c $< -o $@

start.o: start.S
	$(CC) -c start.S

clean:
	rm -f *.o kernel8.elf kernel8.img

run: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial mon:stdio

tty: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial "pty"
