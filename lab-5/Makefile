ARMGNU ?= aarch64-linux-gnu
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -g -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only
ASMOPS = -Iinclude
BUILD_DIR = build
SRC_DIR = src

all: clean kernel8.img

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(CFLAGS) -MMD -c $< -o $@ -g

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@
	
C_FILES = $(wildcard $(SRC_DIR)/*.c) 
ASM_FILES = $(wildcard $(SRC_DIR)/*.S) 
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	$(ARMGNU)-ld -nostdlib -nostartfiles $(OBJ_FILES) -T $(SRC_DIR)/linker.ld  -o $(BUILD_DIR)/kernel8.elf 
	$(ARMGNU)-objcopy -O binary $(BUILD_DIR)/kernel8.elf kernel8.img

clean:
	rm -rf $(BUILD_DIR)
	rm kernel8.elf *.o >/dev/null 2>/dev/null || true

run:
	make all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio -display none

int:
	make all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio -display none -d int

debug: 
	make all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio -display none -S -s -d int

gtk:
	make all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img