SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -O0 -ffreestanding -nostdinc -nostdlib -nostartfiles 
CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OC = aarch64-linux-gnu-objcopy

all: clean kernel8.img

start.o: start.S
	$(CC) $(CFLAGS) -c start.S -o start.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel8.img: start.o $(OBJS)
#	$(LD) -nostdlib -nostartfiles start.o $(OBJS) -o kernel8.elf
	$(LD) -nostdlib -nostartfiles start.o $(OBJS) -T linker.ld -o kernel8.elf
	$(OC) -O binary kernel8.elf needed_kernel8.img

clean:
	rm kernel8.elf *.o >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3 -kernel needed_kernel8.img -serial null -serial stdio

debug:
	qemu-system-aarch64 -M raspi3 -kernel needed_kernel8.img -serial null -serial stdio -S -s
