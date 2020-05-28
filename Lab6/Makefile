ARMGNU ?= aarch64-linux-gnu
COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Wextra -Werror 
BUILD_DIR = build
SRC_DIR = src

all: kernel8.img
clean:
	rm -rf $(BUILD_DIR) *.img *.elf rd prog

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -g -c  $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc  -g -c  $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S) 
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES) rd prog
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o kernel8.elf $(OBJ_FILES) rd prog
	$(ARMGNU)-objcopy kernel8.elf -O binary kernel8.img

src/user/user.o: src/user/user.c
	 $(ARMGNU)-gcc $(COPS) -fno-zero-initialized-in-bss -g -c src/user/user.c -o src/user/user.o

src/user/user_lib.o: src/user/user_lib.S
	$(ARMGNU)-gcc $(COPS) -g -c src/user/user_lib.S -o src/user/user_lib.o

rd: src/user/user.o src/user/user_lib.o build/string_c.o 
	$(ARMGNU)-ld -T src/user/linker.ld -o user.elf src/user/user.o src/user/user_lib.o build/string_c.o 
	$(ARMGNU)-objcopy user.elf -O binary user.img
	$(ARMGNU)-ld -r -b binary user.img -o rd

prog: src/user/user.o rd
	$(ARMGNU)-ld -r -b binary user.elf -o prog

