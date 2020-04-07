CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -O2 -nostdlib -nostdinc -nostartfiles -ffreestanding
# -ffreestanding directs the compiler to not assume that standard functions have their usual definition.
.PHONY: all run clean
all: kernel8.img

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel8.img: start.o branch.o $(OBJS) 
	$(LD) -nostdlib -nostatfiles start.o branch.o $(OBJS) -T link.ld -o kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

clean:
	rm -rf kernel8.* *.o 

run: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial pty -display none 
