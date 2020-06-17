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
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img  -serial null -serial stdio -display none #-d in_asm

user/user.o: user/user.c
	$(CC) $(CFLAGS) -fno-zero-initialized-in-bss -g -c user/user.c -o user/user.o

user/user_lib.o: user/user_lib.S
	$(CC) $(CFLAGS) -g -c user/user_lib.S -o user/user_lib.o

rd: user/user.o user/user_lib.o
	$(LD) -T src/user/linker.ld -o user.elf user/user.o user/user_lib.o 
	$(OBJCOPY) user.elf -O binary user.img
	$(LD) -r -b binary user.img -o rd

test_rb: user/user_lib.o
	$(CC) $(CFLAGS) -fno-zero-initialized-in-bss -g -c user/test.c -o user/test.o
	$(LD) -T user/linker.ld -o test.elf user/test.o  user/user_lib.o
	$(OBJCOPY) test.elf -O binary test.img
	$(LD) -r -b binary test.img -o test_rb
