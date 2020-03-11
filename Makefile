CFLAGS = -Wall

all: kernel8.img

start.o: start.S
	aarch64-linux-gnu-gcc $(CFLAG) -c start.S -o start.o

kernel8.img: start.o link.ld
	aarch64-linux-gnu-ld -nostdlib -nostartfiles start.o -T link.ld -o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img 

clean:
	rm kernel8.img kernel8.elf *.o
run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -d in_asm
