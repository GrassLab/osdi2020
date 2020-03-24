TOOLCHAIN_PREFIX ?= aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)ld
OBJCP = $(TOOLCHAIN_PREFIX)objcopy
QEMU = qemu-system-aarch64
LDSCRIPT = linker.ld
LDFLAGS = -nostartfiles
CFLAGS = -Wall -O2  -ffreestanding -nostdlib -nostartfiles # -mgeneral-regs-only

SRC = $(wildcard *.c)
OBJS = $(SRC:.c=.o)
TARGET = kernel8.img
ELF_TARGET = kernel8.elf

.PHONY: all clean

all: $(SRC) $(TARGET)

# create object files
start.o: start.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(TESTFLAGS) $(CFLAGS) -c $< -o $@

# create image file from elf by using objcopy
$(TARGET): $(ELF_TARGET)
	$(OBJCP) -O binary $(ELF_TARGET) $@

# create the elf file
$(ELF_TARGET): start.o $(OBJS)
	$(LD) $(LDFLAGS) start.o $(OBJS) -T $(LDSCRIPT) -o $(ELF_TARGET)

run: $(TARGET)
	$(QEMU) -M raspi3 -kernel $(TARGET) -serial stdio

run-asm: $(TARGET)
	$(QEMU) -M raspi3 -kernel $(TARGET) -d in_asm

test:
	make clean
	make "TESTFLAGS+=-DTEST -DSHOW"

install:
	sudo mount /dev/sdc1 ~/Disk
	sudo cp kernel8.img ~/Disk/kernel8.img
	sudo umount ~/Disk


clean:
	$(RM) *.o $(TARGET) $(ELF_TARGET) $(OBJS)
