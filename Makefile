ARMGNU ?= aarch64-linux-gnu

ASMOPS = -Iinclude -g
COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only -std=c11 -g

QEMU     = qemu-system-aarch64

IMG = kernel8.img
SRC_DIR = src
BUILD_DIR = build
C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

all : $(IMG)

test:
	make clean
	make CFLAGS+=-DTEST

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(CFLAGS) $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

$(IMG): $(SRC_DIR)/linker.ld $(OBJ_FILES)
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary $(IMG)


run: $(IMG)
	$(QEMU) -serial null -serial stdio -M raspi3 -kernel $(IMG) -display none

runasm: $(IMG)
	$(QEMU) -serial null -serial stdio -M raspi3 -kernel $(IMG) -display none -d in_asm

gdb: $(IMG)
	$(QEMU) -M raspi3 -kernel $(IMG) -display none -S -s

update: $(IMG)
	sudo mount /dev/sdc1 /mnt && sudo cp $(IMG) /mnt && sudo umount /mnt

clean :
	rm -rf $(BUILD_DIR) *.img 
