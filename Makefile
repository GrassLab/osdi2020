SHELL = /bin/sh
ARM = aarch64-linux-gnu
CC = $(ARM)-gcc
LD = $(ARM)-ld
IDIR = inc
SDIR = src
BDIR = build
CFLAGS = -Wall -I $(IDIR) -O0
OBJCOPY = $(ARM)-objcopy
S_SRCS = $(wildcard $(SDIR)/*.S)
C_SRCS = $(wildcard $(SDIR)/*.c)
S_OBJS = $(S_SRCS:$(SDIR)/%.S=$(BDIR)/%.asmo)
C_OBJS = $(C_SRCS:$(SDIR)/%.c=$(BDIR)/%.o)

all: clean kernel8.img

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

kernel8.elf: $(S_OBJS) linker.ld $(C_OBJS) rd prog
	$(LD) -T linker.ld -o kernel8.elf $(S_OBJS) $(C_OBJS) rd prog

user/user.o: user/user.c
	$(CC) $(CFLAGS) -fno-zero-initialized-in-bss -c user/user.c -o user/user.o

user/user_lib.o: user/user_lib.S
	$(CC) $(CFLAGS) -c user/user_lib.S -o user/user_lib.o

rd: user/user.o user/user_lib.o
	$(LD) -T user/linker.ld -o user.elf user/user.o user/user_lib.o
	$(OBJCOPY) user.elf -O binary user.img
	$(LD) -r -b binary user.img -o rd

prog: user/user.o rd
	$(LD) -r -b binary user.elf -o prog

$(BDIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BDIR)/%.asmo: $(SDIR)/%.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BDIR)/*.asmo $(BDIR)/*.o kernel8.elf kernel8.img rd prog user/user.o user/user_lib.o user.img user.elf

run: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial mon:stdio

tty: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial "pty"

debug: all
	terminator -e "qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial mon:stdio -s -S" --new-tab
	terminator -e "aarch64-linux-gnu-gdb -x debug.txt" --new-tab
