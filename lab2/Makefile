ARMGNU = aarch64-linux-gnu
COPS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -g
INCLUDE = /usr/aarch64-linux-gnu/include
SRCS = $(wildcard *.c)
LIB_ASM = $(wildcard *.s)
OBJS = $(SRCS:.c=.o) \
       $(LIB_ASM:.s=.o)

.PHONY:
	clean all run

all: kernel8.img

start.o: start.S
	$(ARMGNU)-gcc -I $(COPS) -MMD -c $< -o $@

%.o: %.s
	$(ARMGNU)-gcc -o $@ -c $<

%.o: %.c
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

kernel8.img: start.o $(OBJS)
	$(ARMGNU)-ld -T linker.ld -o kernel8.elf start.o $(OBJS) 
	$(ARMGNU)-objcopy -O binary kernel8.elf $@

clean:
	rm *.o *.d
	rm kernel8.*

run_d:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial pty -S -s

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial pty
