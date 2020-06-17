ARMGNU ?= aarch64-linux-gnu
CFLAGS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only -Iinc -g
ASMOPS = -Iinclude -g
BUILD_DIR = build
SRC_DIR = src
LIB_DIR = lib

C_FILES = $(wildcard $(SRC_DIR)/*.c) 
ASM_FILES = $(wildcard $(SRC_DIR)/*.S) 
C_LIB_FILES = $(wildcard $(LIB_DIR)/*.c) 
ASM_LIB_FILES = $(wildcard $(LIB_DIR)/*.S) 
C_OBJS = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o) $(C_LIB_FILES:$(LIB_DIR)/%.c=$(BUILD_DIR)/%_c.o)
S_OBJS = $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o) $(ASM_LIB_FILES:$(LIB_DIR)/%.S=$(BUILD_DIR)/%_s.o)
DEP_FILES = $(C_OBJS:%.o=%.d)
DEP_FILES += $(S_OBJS:%.o=%.d)
-include $(DEP_FILES)

all: clean kernel8.img

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(CFLAGS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_c.o: $(LIB_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(CFLAGS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(LIB_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@



kernel8.img: $(SRC_DIR)/linker.ld $(C_OBJS) $(S_OBJS)
	$(ARMGNU)-ld -nostdlib -nostartfiles $(S_OBJS) $(C_OBJS) -T $(SRC_DIR)/linker.ld  -o kernel8.elf 
	$(ARMGNU)-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm -rf $(BUILD_DIR)
	rm kernel8.elf *.o >/dev/null 2>/dev/null || true

run:
	make all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio -display none

run_excp:
	make all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio -display none -d int

pipe:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial  pipe:/tmp/guest -S -s

debug: 
	make all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio -display none -S -s -d int

gtk:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img

gdb:
	gdb-multiarch -x cmd.gdb