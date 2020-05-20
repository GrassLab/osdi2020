ARMGNU ?= aarch64-linux-gnu

COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only -std=gnu99 -ggdb
ASMOPS = -Iinclude -ggdb

BUILD_DIR = build
SRC_DIR = src

all : kernel8.img rd

clean :
	rm -rf $(BUILD_DIR) *.img rd user/*.o *.elf

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

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES) rd
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES) rd
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img

user/user.o: user/user.c
	 $(ARMGNU)-gcc -Os $(COPS) -fno-zero-initialized-in-bss -g -c $< -o $@

user/user_lib_s.o: user/user_lib.S
	$(ARMGNU)-gcc -Os $(COPS) -g -c $< -o $@

user/user_lib_c.o: user/user_lib.c
	$(ARMGNU)-gcc -Os $(COPS) -g -c $< -o $@

rd: user/user.o user/user_lib_s.o user/user_lib_c.o
	$(ARMGNU)-ld -T user/linker.ld -o user.elf $^
	$(ARMGNU)-objcopy user.elf -O binary user.img
	$(ARMGNU)-ld -r -b binary user.img -o rd

stdio: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel $< -serial null -serial stdio -display none

gdb: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s
