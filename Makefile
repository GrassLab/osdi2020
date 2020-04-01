TOOLCHAIN_PREFIX = aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)ld
OBJCPY = $(TOOLCHAIN_PREFIX)objcopy

BUILD_DIR = build
SRC_DIR = src
OUT_DIR = $(BUILD_DIR)/kernel
BL_SRC_DIR = bootloader
BL_OUT_DIR = $(BUILD_DIR)/bootloader

LINKER_FILE = $(SRC_DIR)/linker.ld
ENTRY = $(SRC_DIR)/start.s
ENTRY_OBJS = $(OUT_DIR)/start.o
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OUT_DIR)/%.o)

CFLAGS = -Wall -I include -c

BL_LINKER_FILE = $(BL_SRC_DIR)/linker.ld
BL_ENTRY = $(BL_SRC_DIR)/start.s
BL_ENTRY_OBJS = $(BL_OUT_DIR)/start.o
BL_SRCS = $(wildcard $(BL_SRC_DIR)/*.c)
BL_OBJS = $(BL_SRCS:$(BL_SRC_DIR)/%.c=$(BL_OUT_DIR)/%.o)

BL_CFLAGS = -Wall -I $(BL_SRC_DIR)/include -c

.PHONY: all clean asm debug build_dir kernel bootloader

all: build_dir bootloader.img kernel8.img

# build kernel

$(ENTRY_OBJS): $(ENTRY)
	$(CC) $(CFLAGS) $< -o $@

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

kernel8.img: $(OBJS) $(ENTRY_OBJS)
	$(LD) $(ENTRY_OBJS) $(OBJS) -T $(LINKER_FILE) -o kernel8.elf
	$(OBJCPY) -O binary kernel8.elf kernel8.img

# build bootloader

$(BL_ENTRY_OBJS): $(BL_ENTRY)
	$(CC) $(BL_CFLAGS) $< -o $@

$(BL_OUT_DIR)/%.o: $(BL_SRC_DIR)/%.c
	$(CC) $(BL_CFLAGS) $< -o $@

bootloader.img: $(BL_OBJS) $(BL_ENTRY_OBJS)
	$(LD) $(BL_ENTRY_OBJS) $(BL_OBJS) -T $(BL_LINKER_FILE) -o bootloader.elf
	$(OBJCPY) -O binary bootloader.elf bootloader.img

# run emulator

kernel: $(OUT_DIR) kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio

bootloader: $(BL_OUT_DIR) bootloader.img
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -serial pty -display none

asm:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm

debug: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s

# utility 

build_dir: $(OUT_DIR) $(BL_OUT_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(BL_OUT_DIR):
	mkdir -p $(BL_OUT_DIR)

clean:
	rm -rf $(BUILD_DIR)/*
	rm -f *.elf *.img
