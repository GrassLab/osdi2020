SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -fPIC -Wall -ffreestanding -nostdinc -nostdlib -nostartfiles -Iinclude -MMD
ASMFLAGS = -fPIC -Iinclude -MMD

all: clean kernel8.img

start.o: start.S
	aarch64-linux-gnu-gcc $(ASMFLAGS) -c start.S -o start.o

%.o: %.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

kernel8.img: start.o $(OBJS)
	aarch64-linux-gnu-ld -T link.ld -o kernel8.elf start.o $(OBJS)
	aarch64-linux-gnu-objcopy kernel8.elf -O binary kernel8.img

clean:
	rm kernel8.elf *.o >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio
