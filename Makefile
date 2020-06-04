CC			= aarch64-linux-gnu-gcc
LD 			= aarch64-linux-gnu-ld
OBJ_CPY 	= aarch64-linux-gnu-objcopy
OBJ_DUMP	= aarch64-linux-gnu-objdump
EMULATOR	= qemu-system-aarch64

CFLAGS 		= -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -Werror -Wextra
INCLUDE_FLG = -Iinclude

BUILD_DIR			= build
DEBUG_DIR   		= debug

LINKER_FILE	= link.ld
ELF_FILE	= $(BUILD_DIR)/kernel8.elf

SRCS		= $(wildcard src/*.c)
ASMS   		= $(wildcard src/*.S)
SRC_OBJS	= $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))
ASM_OBJS 	= $(patsubst %.S, $(BUILD_DIR)/%.S.o, $(ASMS))
OBJS   		= $(SRC_OBJS) $(ASM_OBJS)

.PHONY: all clean run force generate_symbol_asm

all: kernel8.img
force: clean kernel8.img

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(DEBUG_DIR)

kernel8.img: $(ELF_FILE) generate_symbol_asm
	$(OBJ_CPY) -O binary $< $@

$(ELF_FILE) : $(OBJS)
	$(LD) -nostdlib -nostartfiles  -T $(LINKER_FILE) $^ -o $@

$(BUILD_DIR)/%.S.o: %.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE_FLG) -c $< -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE_FLG) -c $< -o $@

run: kernel8.img
	$(EMULATOR) -M raspi3 -kernel kernel8.img -serial stdio

generate_symbol_asm:
	@rm -rf $(DEBUG_DIR)
	@mkdir $(DEBUG_DIR)
	readelf --all $(ELF_FILE) > $(DEBUG_DIR)/kernel_symbol
	$(OBJ_DUMP) -d $(ELF_FILE)  > $(DEBUG_DIR)/kernel_asm	

debug: CFLAGS += -ggdb3 -Og

debug: clean kernel8.img generate_symbol_asm
	$(EMULATOR) -M raspi3 -kernel kernel8.img -serial stdio -s -S

