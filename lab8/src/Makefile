CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -nostdlib -nostdinc -nostartfiles -ffreestanding -MMD
ASMOPS = -MMD
# -ffreestanding directs the compiler to not assume that standard functions have their usual definition.
.PHONY: all run clean
all: kernel8.img

%_s.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel8.img: start_s.o branch_s.o entry_s.o irq_s.o utils_s.o sched_s.o mm_s.o sys_s.o $(OBJS) 
	$(LD) -nostdlib -nostatfiles start_s.o  entry_s.o irq_s.o utils_s.o sched_s.o mm_s.o sys_s.o $(OBJS) -T link.ld -o kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

clean:
	rm -rf kernel8.* *.o *.d

run: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img  -serial null -serial stdio -display none -drive if=sd,file=test.img,format=raw
#-d in_asm
