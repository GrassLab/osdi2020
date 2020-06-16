TOOLCHAIN_PREFIX = aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)ld
OBJCPY = $(TOOLCHAIN_PREFIX)objcopy

BUILD_DIR = build
SRC_DIR = src
LIB_DIR = lib
USER_DIR = user

LIB_C = $(wildcard $(LIB_DIR)/*.c)
LIB_OBJS_FILES = $(LIB_C:$(LIB_DIR)/%.c=$(BUILD_DIR)/lib/%_c.o)

SRC_C = $(wildcard $(SRC_DIR)/*.c)
SRC_ASM = $(wildcard $(SRC_DIR)/*.S)
SRC_OBJS_FILES = $(SRC_C:$(SRC_DIR)/%.c=$(BUILD_DIR)/src/%_c.o)
SRC_OBJS_FILES += $(SRC_ASM:$(SRC_DIR)/%.S=$(BUILD_DIR)/src/%_asm.o)

USER_C = $(wildcard $(USER_DIR)/*.c)
USER_ASM = $(wildcard $(USER_DIR)/*.S)
USER_OBJS_FILES = $(USER_C:$(USER_DIR)/%.c=$(BUILD_DIR)/user/%_c.o)
USER_OBJS_FILES += $(USER_ASM:$(USER_DIR)/%.S=$(BUILD_DIR)/user/%_asm.o)

NO_BUILT_IN = -fno-builtin-printf -fno-builtin-memcpy -fno-builtin-strcpy
CFLAGS = -Wall -Wextra -nostdlib -nostdinc $(NO_BUILT_IN) -Iinclude -Ilib -c #-Werror
USER_CFLAGS = -Wall -Wextra -nostdlib -nostdinc -fno-zero-initialized-in-bss $(NO_BUILT_IN) -Ilib -c -Werror

.PHONY: all clean

all: build_dir kernel8.img

# build library

$(BUILD_DIR)/lib/%_c.o: $(LIB_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

# build kernel

$(BUILD_DIR)/src/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/src/%_asm.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

kernel8.img: $(SRC_OBJS_FILES) $(LIB_OBJS_FILES) user_embed.elf
	$(LD) $(SRC_OBJS_FILES) $(LIB_OBJS_FILES) user_embed.elf -T $(SRC_DIR)/linker.ld -o kernel8.elf
	$(OBJCPY) -O binary kernel8.elf kernel8.img

# build user library

$(BUILD_DIR)/user/%_c.o: $(USER_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(USER_CFLAGS) $< -o $@

$(BUILD_DIR)/user/%_asm.o: $(USER_DIR)/%.S
	mkdir -p $(@D)
	$(CC) $(USER_CFLAGS) $< -o $@

user_embed.elf: $(USER_OBJS_FILES) $(LIB_OBJS_FILES)
	$(LD) $(USER_OBJS_FILES) $(LIB_OBJS_FILES) -T $(USER_DIR)/linker.ld -o user.elf
	$(OBJCPY) -O binary user.elf user.img
	$(LD) -r -b binary user.img -o user_embed.elf

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
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio -display none -S -s

# utility 

build_dir: $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/*
	rm -f *.elf *.img
