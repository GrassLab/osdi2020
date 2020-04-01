ARMGNU ?= aarch64-linux-gnu
COPS = -O2 -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude
ASMOPS = -Iinclude 
BUILD_DIR = build
SRC_DIR = src

all: kernel8.img
clean:
	rm -rf $(BUILD_DIR) *.img

# if x.c in src directory, then compile it in build directory
# $@ (full target name of the current target)
# $< (name of the first dependency)
# -MMD parameter instructs the gcc compiler to create a dependency file for each generated object file
$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS)  -c $< -o $@

# if x.s in src directory, then compile it in build directory
$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS)  -c $< -o $@

# building an array of all object file from the concatenation of both C and assembler source files.
C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S) 
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

# We need to include all of the generated dependency files so that make knows what exactly to recompile in case a header changes.
DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

# use the OBJ_FILES array to build the kernel8.elf file. 
# use the linker script src/linker.ld to define the basic layout of the resulting executable image
kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o kernel8.elf $(OBJ_FILES)

# ELF files are designed to be executed by an operating system. To write a bare-metal program, we need to extract all executable and data sections from the ELF file and put them into the kernel8.img image
	$(ARMGNU)-objcopy kernel8.elf -O binary kernel8.img
