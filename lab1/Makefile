SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFALG = -Wall -ffreestanding -nostdinc -nostdlib -nostartfiles

all: kernel8.img

start.o: start.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c start.S -o start.o

%.o: %.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

kernel8.img: start.o $(OBJS)
	aarch64-linux-gnu-ld -nostdlib -nostartfiles start.o $(OBJS) -T link.ld -o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.elf *.o

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -d in_asm


