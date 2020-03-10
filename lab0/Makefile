TOOLCHAIN_PREFIX ?= aarch64-linux-gnu-

CC = $(TOOLCHAIN_PREFIX)gcc
OBJCP = $(TOOLCHAIN_PREFIX)objcopy
QEMU = qemu-system-aarch64
LDSCRIPT = linker.ld
CFLAGS = -nostdlib -Wl,-T,$(LDSCRIPT)

SRC = $(wildcard *.S)
TARGET = kernel8.img
ELF_TARGET = kernel8.elf

.PHONY: all clean

all: $(SRC) $(TARGET)

$(TARGET): $(ELF_TARGET)
	$(OBJCP) -O binary $^ $@

$(ELF_TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) $(TARGET) $(ELF_TARGET)

qemu: $(TARGET)
	 $(QEMU) -M raspi3 -kernel $^ -display none -d in_asm
