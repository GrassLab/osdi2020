ARMGNU = aarch64-linux-gnu
COPS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
INCLUDE = /usr/aarch64-linux-gnu/include
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

.PHONY:
	clean all run

all: kernel8.img

start.o: start.S
	$(ARMGNU)-gcc -I $(INCLUDE) $(COPS) -c $< -o $@

%.o: %.c
	$(ARMGNU)-gcc -I $(INCLUDE)  $(COPS) -c $< -o $@

kernel8.img: start.o $(OBJS)
	$(ARMGNU)-ld -T linker.ld -o kernel8.elf start.o $(OBJS) 
	$(ARMGNU)-objcopy -O binary kernel8.elf $@

clean:
	rm *.o
	rm kernel8.*

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio
