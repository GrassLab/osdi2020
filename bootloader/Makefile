TOOLCHAIN_PREFIX = aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)ld
OBJCPY = $(TOOLCHAIN_PREFIX)objcopy

BUILD_DIR = build
SRC_DIR = src

LINKER_FILE = $(SRC_DIR)/linker.ld
ENTRY = $(SRC_DIR)/start.s
ENTRY_OBJS = $(BUILD_DIR)/start.o
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

CFLAGS = -Wall -I include -c

.PHONY: all clean

all: build_dir kernel8.img

# build

$(ENTRY_OBJS): $(ENTRY)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

kernel8.img: $(OBJS) $(ENTRY_OBJS)
	$(LD) $(ENTRY_OBJS) $(OBJS) -T $(LINKER_FILE) -o kernel8.elf
	$(OBJCPY) -O binary kernel8.elf kernel8.img 

# run emulator

run: $(BUILD_DIR) kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial stdio

display: $(BUILD_DIR) kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio

tty: $(BUILD_DIR) kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial pty

asm:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm

debug: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s

# utility 

build_dir: $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/*
	rm -f *.elf *.img
