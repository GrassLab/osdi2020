SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFALG = -Wall -ffreestanding -nostdinc -nostdlib -nostartfiles


all: kernel8.img

kernel8.img: kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img
kernel8.elf: start.o $(OBJS)
	aarch64-linux-gnu-ld -T l.ld -o kernel8.elf start.o $(OBJS)
start.o:
	aarch64-linux-gnu-gcc -c start.S
%.o: %.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

.PHONY: clean run runasm

clean:
	rm -rf  *.o
	rm -rf  *.elf
	rm -rf  *.img

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio

runasm:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm -serial null -serial stdio
