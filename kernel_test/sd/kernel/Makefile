ARMGNU = aarch64-linux-gnu
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -O2 -ffreestanding -fno-stack-protector -nostdinc -nostdlib -nostartfiles

all: clean kernel8.img

start.o: start.S
	$(ARMGNU)-gcc $(CFLAGS) -c start.S -o start.o

%.o: %.c
	$(ARMGNU)-gcc $(CFLAGS) -c $< -o $@

kernel8.img: start.o $(OBJS)
	$(ARMGNU)-ld -nostdlib -nostartfiles start.o $(OBJS) -T link.ld -o kernel8.elf
	$(ARMGNU)-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.elf *.o >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio

objdump:
	$(ARMGNU)-objdump -d kernel8.elf

readelf:
	$(ARMGNU)-readelf -a kernel8.elf

gdb:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial stdio -S -s
