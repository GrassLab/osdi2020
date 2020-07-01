TOOLCHAIN_PREFIX ?= aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)ld
OBJCP = $(TOOLCHAIN_PREFIX)objcopy
QEMU = qemu-system-aarch64
LDSCRIPT = kernel/linker.ld
LDFLAGS = -nostartfiles
CFLAGS = -Wall -O2 -fPIC -ffreestanding -nostdlib -nostartfiles -Iinclude -Iperipheral -mgeneral-regs-only

PeriDir   = peripheral
KernelDir = kernel

SRC = $(wildcard */*.c)
ASM = $(wildcard */*.S)

OBJS = $(SRC:%.c=build/%_c.o)
OBJS += $(ASM:%.S=build/%_s.o)


TARGET = kernel8.img
ELF_TARGET = kernel8.elf

.PHONY: all clean

all: $(TARGET)

# create the object files
build/%_c.o: %.c
	@mkdir -p $(@D)
	$(CC) $(TESTFLAGS) $(CFLAGS) -c $< -o $@

build/%_s.o: %.S
	@mkdir -p $(@D)
	$(CC) $(TESTFLAGS) $(CFLAGS) -c $< -o $@

# create image file from elf by using objcopy
$(TARGET): $(ELF_TARGET)
	@mkdir -p $(@D)
	$(OBJCP) -O binary $(ELF_TARGET) $@

# create the elf file
$(ELF_TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -T $(LDSCRIPT) -o $(ELF_TARGET)

run: $(TARGET)
	$(QEMU) -M raspi3 -kernel $(TARGET) -serial pty

stdio: $(TARGET)
	$(QEMU) -M raspi3 -kernel $(TARGET) -serial null -serial stdio -display none -drive if=sd,file=sfn_nctuos.img,format=raw

stdio_int: $(TARGET)
	$(QEMU) -M raspi3 -kernel $(TARGET) -display none -serial null -serial stdio -d int

debug: $(TARGET)
	$(QEMU) -M raspi3 -kernel $(TARGET) -display none -S -s

test:
	make clean
	make "TESTFLAGS+=-DTEST -DSHOW"

clean:
	$(RM) $(TARGET) $(ELF_TARGET) $(OBJS)
