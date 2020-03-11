SHELL = /bin/sh
CC:=aarch64-linux-gnu-gcc
LD:=aarch64-linux-gnu-ld
OBJCOPY:=aarch64-linux-gnu-objcopy

all: kernel8.img

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

kernel8.elf: a.o linker.ld
	$(LD) -T linker.ld -o kernel8.elf a.o

a.o: a.S
	$(CC) -c a.S

clean: a.o kernel8.elf kernel8.img
	rm -f a.o kernel8.elf kernel8.img

