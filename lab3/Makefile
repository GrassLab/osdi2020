ARMGNU ?= aarch64-linux-gnu

# -Iinclude tells it to look for header files in the include folder
#COPS = -fPIC -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only
COPS = -ggdb -fPIC -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -Wno-unused-variable
ASMOPS = -ggdb -fPIC -Iinclude -Wno-unused-variable

BUILD_DIR = build
SRC_DIR = src

all : kernel8.img

clean :
	rm -rf $(BUILD_DIR) *.img

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c) 
ASM_FILES = $(wildcard $(SRC_DIR)/*.S) 
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

kernel8.img: $(SRC_DIR)/link.ld $(OBJ_FILES)
	$(ARMGNU)-ld -T $(SRC_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES) -Map System.map
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img

#debug:	# for debugging
#	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s

#debug:
#	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio -s -S

debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -gdb tcp::8888 -S

stdio_run:	# PL011(uart0)
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio

pty_run:	# connect to /dev/pts/{number}
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial pty
