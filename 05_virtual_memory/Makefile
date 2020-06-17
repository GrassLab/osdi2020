ARMGNU ?= aarch64-linux-gnu

COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only
ASMOPS = -Iinclude 

BUILD_DIR = build
SRC_DIR = src
USER_DIR = $(SRC_DIR)/user
BIN_DIR = bin

all : build $(BUILD_DIR)/kernel8.img 

clean :
	rm -rf $(BUILD_DIR) *.img $(BIN_DIR)

build:
	mkdir -p build
	mkdir -p bin

run: all
	qemu-system-aarch64 -M raspi3 -kernel $(BUILD_DIR)/kernel8.img -serial null -serial stdio -display none -d in_asm

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

$(USER_DIR)/user.o: $(USER_DIR)/user.c
	$(ARMGNU)-gcc $(COPS) -fno-zero-initialized-in-bss -g -c -I$(USER_DIR) $(USER_DIR)/user.c -o $(USER_DIR)/user.o

$(USER_DIR)/user_lib.o: $(USER_DIR)/user_lib.S
	$(ARMGNU)-gcc $(COPS) -g -c $(USER_DIR)/user_lib.S -o $(USER_DIR)/user_lib.o

$(BIN_DIR)/rd: $(USER_DIR)/user.o $(USER_DIR)/user_lib.o
	$(ARMGNU)-ld -T $(USER_DIR)/linker.ld -o $(BUILD_DIR)/user.elf $(USER_DIR)/user.o $(USER_DIR)/user_lib.o 
	$(ARMGNU)-objcopy $(BUILD_DIR)/user.elf -O binary $(BUILD_DIR)/user.img
	$(ARMGNU)-ld -r -b binary $(BUILD_DIR)/user.img -o $(BIN_DIR)/rd

$(BIN_DIR)/prog: $(USER_DIR)/user.o $(BIN_DIR)/rd
	$(ARMGNU)-ld -r -b binary $(BUILD_DIR)/user.elf -o $(BIN_DIR)/prog

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

$(BUILD_DIR)/kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES) $(BIN_DIR)/rd $(BIN_DIR)/prog
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES) $(BIN_DIR)/rd $(BIN_DIR)/prog
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary $(BUILD_DIR)/kernel8.img 
