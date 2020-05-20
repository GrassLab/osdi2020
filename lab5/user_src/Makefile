ARMGNU ?= aarch64-linux-gnu
ASMOPS = -fPIC -Iinclude -g
COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only -std=c11 -g -fPIC -fno-zero-initialized-in-bss

COPS += -DON_QEMU
ASMOPS += -DON_QEMU

USER_SRC_DIR = .
USER_BUILD_DIR = ../user_build
USER_C_FILES = $(wildcard $(USER_SRC_DIR)/*.c)
USER_ASM_FILES = $(wildcard $(USER_SRC_DIR)/*.S)
USER_OBJ_FILES = $(USER_C_FILES:$(USER_SRC_DIR)/%.c=$(USER_BUILD_DIR)/%_c.o)
USER_OBJ_FILES += $(USER_ASM_FILES:$(USER_SRC_DIR)/%.S=$(USER_BUILD_DIR)/%_s.o)

all: $(USER_BUILD_DIR)/user.o

$(USER_BUILD_DIR):
	mkdir $(USER_BUILD_DIR)

$(USER_BUILD_DIR)/%_c.o: $(USER_SRC_DIR)/%.c
	$(ARMGNU)-gcc $(CFLAGS) $(COPS) -MMD -c $< -o $@

$(USER_BUILD_DIR)/%_s.o: $(USER_SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

$(USER_BUILD_DIR)/user.img: $(USER_BUILD_DIR) $(USER_OBJ_FILES)
	$(ARMGNU)-ld -T $(USER_SRC_DIR)/linker.ld -o $(USER_BUILD_DIR)/user.elf $(USER_OBJ_FILES)
	$(ARMGNU)-objcopy $(USER_BUILD_DIR)/user.elf -O binary $(USER_BUILD_DIR)/user.img

$(USER_BUILD_DIR)/user.o: $(USER_BUILD_DIR)/user.img
	$(ARMGNU)-ld -r -b binary $(USER_BUILD_DIR)/user.img -o $(USER_BUILD_DIR)/user.o

clean:
	rm -rf $(USER_BUILD_DIR)
