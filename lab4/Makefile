SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFALG = -fPIC -Wall -ffreestanding -nostdinc -nostdlib -nostartfiles -Iinclude -g -O0 -ggdb3
ASMFALG = -fPIC -Iinclude -g -O0 -ggdb3
SRC_DIR = src
BUILD_DIR = build

all: kernel8.img


$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	aarch64-linux-gnu-gcc $(CFALG) -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	aarch64-linux-gnu-gcc $(ASMFALG) -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

build_dir:
	mkdir -p build

kernel8.img: build_dir $(OBJ_FILES)
	aarch64-linux-gnu-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES)
	aarch64-linux-gnu-objcopy -O binary $(BUILD_DIR)/kernel8.elf kernel8.img



.PHONY: clean run runasm runpty

clean:
	rm -rf $(BUILD_DIR) *.img 

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial stdio

runpty:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial pty

runasm:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm -serial stdio


rundis:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio

rungdb:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial stdio -S -gdb tcp::1234
