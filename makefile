SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -fpie -pie -fno-stack-protector -nostdlib -nostartfiles -ffreestanding

all: kernel8.img

start.o: start.S
	aarch64-linux-gnu-gcc -c start.S

kearnel8.elf kernel8.img: start.o $(OBJS)
	aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf start.o $(OBJS)
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

%.o: %.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@ $(CFLAGS)

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio -display none

run-detail:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio

run-mini-uart:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial pty

run-uart0:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial pty

run-script:
	sudo python script.py

connect:
	sudo screen /dev/ttyUSB0 115200

check-elf:
	aarch64-linux-gnu-readelf -s kernel8.elf

clean:
	rm -rf *.o kernel8.*
