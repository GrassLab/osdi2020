ARMGNU ?= aarch64-linux-gnu

ASMOPS = -fPIC -Iinclude -g
COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only -std=c11 -g
#COPS += -DMINIUART

QEMU     = qemu-system-aarch64

SRC_DIR = src
BUILD_DIR = build
C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEFAULT_TARGET = kernel8
DEFAULT_KERNEL_LOC = 0x80000
DEFAULT_STAMP_LOC = 0x80000
#DEFAULT_STAMP_LOC = 0x200000
LINKER_SCRIPT = $(SRC_DIR)/linker.ld
IMG = $(DEFAULT_TARGET).img


# boot image flags
# -DMOVPI   moving rpi boot img
# -DEXAMPLE HOMER
# -DNORPI   only black white tabs
# no flag   RPI logo
#STAMP_OPT= -DWITHOUT_LOADER -DMOVPI
STAMP_OPT= -DNORPI -DNO_RELOC_SELF

all : $(IMG)

test:
	make clean
	make CFLAGS+=-DTEST

stamp: stamp.img

stamp.img:
	$(eval LOC := $(if $(LOC),$(LOC), $(DEFAULT_STAMP_LOC)))
	#make CFLAGS+=-DBUILD_STAMP='"$(shell date -u)"' KERNEL_LOC='"$(LOC)"' TARGET='"stamp"' BUILD_DIR="stamp_build"
	make C'FLAGS+=-DBUILD_STAMP="$(shell date -u)" $(STAMP_OPT)' KERNEL_LOC='"$(LOC)"' TARGET='"stamp"' BUILD_DIR="stamp_build"

showtest:
	make clean
	make "CFLAGS+=-DTEST -DSHOW"

$(BUILD_DIR):
	mkdir $@

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	$(ARMGNU)-gcc $(CFLAGS) $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

$(IMG): $(BUILD_DIR) $(OBJ_FILES)
	$(eval KERNEL_LOC := $(if $(KERNEL_LOC),$(KERNEL_LOC), $(DEFAULT_KERNEL_LOC)))
	$(eval TARGET := $(if $(TARGET),$(TARGET), $(DEFAULT_TARGET)))
	@echo "_kbeg = $(KERNEL_LOC);" > $(LINKER_SCRIPT)
	@cat $(SRC_DIR)/template.ld >> $(LINKER_SCRIPT)
	$(ARMGNU)-ld -T $(LINKER_SCRIPT) -o $(TARGET).elf  $(OBJ_FILES)
	$(ARMGNU)-objcopy $(TARGET).elf -O binary $(TARGET).img

runpl: $(IMG)
	@$(QEMU) -serial stdio -M raspi3 -kernel $(IMG)

runmini: $(IMG)
	@$(QEMU) -serial null -serial stdio -M raspi3 -kernel $(IMG) -display none

run: $(IMG)
	@$(QEMU) -serial stdio -M raspi3 -kernel $(IMG)

runt: stamp.img
	@$(QEMU) -serial stdio -M raspi3 -kernel stamp.img

runasm: $(IMG)
	@$(QEMU) -serial null -serial stdio -M raspi3 -kernel $(IMG) -display none -d in_asm

minitty: $(IMG)
	@$(QEMU) -serial null -serial pty -M raspi3 -kernel $(IMG) -display none

tty: $(IMG)
	@$(QEMU) -serial pty -M raspi3 -kernel $(IMG)

ttyt: stamp.img
	@$(QEMU) -serial pty -M raspi3 -kernel stamp.img

gdb: $(IMG)
	$(QEMU) -serial stdio -M raspi3 -kernel $(IMG) -display none -S -s

update: $(IMG)
	sudo mount /dev/sdc1 /mnt && sudo cp $(IMG) /mnt && sudo umount /mnt

clean :
	rm -rf *$(BUILD_DIR) kernel8.img stamp.img *.elf
