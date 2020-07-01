ARMGNU = aarch64-linux-gnu
COPS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

.PHONY:
	clean all run

all: kernel8.img

start.o: start.S
	$(ARMGNU)-gcc $(COPS) -c $< -o $@

%.o: %.c
	$(ARMGNU)-gcc $(COPS) -c $< -o $@

kernel8.img: start.o $(OBJS)
	$(ARMGNU)-ld -T linker.ld -o kernel8.elf start.o $(OBJS) 
	$(ARMGNU)-objcopy -O binary kernel8.elf $@

clean:
	rm *.o
	rm kernel8.*

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial stdio -drive if=sd,file=sdcard.img,format=raw
