SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
CC = aarch64-linux-gnu-gcc 
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

all: clean kernel8.img

start.o: start.S
	$(CC) $(CFLAGS) -c src/start.S -o src/start.o

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel8.img: src/start.o $(OBJS)
	aarch64-linux-gnu-ld -nostdlib -nostartfiles src/start.o $(OBJS) -T src/link.ld -o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.img kernel8.elf src/*.o >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio
gdb:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio -S -s
objdump:
	aarch64-linux-gnu-objdump -d kernel8.elf
readelf:
	aarch64-linux-gnu-readelf -a kernel8.elf
copy:
	cp kernel8.img /media/hank0438/4DFF-0A36/