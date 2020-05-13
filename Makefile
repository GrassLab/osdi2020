CC			= aarch64-linux-gnu-gcc
LD 			= aarch64-linux-gnu-ld
OBJ_CPY 	= aarch64-linux-gnu-objcopy
OBJ_DUMP	= aarch64-linux-gnu-objdump
EMULATOR	= qemu-system-aarch64

CFLAGS 		= -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -Werror -Wextra
INCLUDE_FLG = -Iinclude -I. -Ilib

BUILD_DIR   = build
DEBUG_DIR   = debug
LINK_SCRIPT = link.ld
ELF_ELE  	= $(BUILD_DIR)/kernel8.elf

SRCS 		= $(wildcard kernel/*/*.c) $(wildcard lib/*.c) $(wildcard src/*.c)
ASMS		= $(wildcard kernel/*/*.S) $(wildcard kernel/*.S) $(wildcard lib/*.S)
SRC_OBJS 	= $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))
ASM_OBJS	= $(patsubst %.S, $(BUILD_DIR)/%.S.o, $(ASMS))

ALL_OBJS	= $(SRC_OBJS) $(ASM_OBJS)

.PHONY: all clean run force

all: kernel8.img
force: clean kernel8.img

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(DEBUG_DIR)

kernel8.img: $(ELF_ELE) 
	$(OBJ_CPY) -O binary $^ $@

$(ELF_ELE) : $(ALL_OBJS)
	$(LD) -nostdlib -nostartfiles $^ -T $(LINK_SCRIPT) -o $@

$(BUILD_DIR)/%.S.o: %.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE_FLG) -c $< -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE_FLG) -c $< -o $@

run: kernel8.img
	$(EMULATOR) -M raspi3 -kernel kernel8.img -serial stdio

debug: CFLAGS += -ggdb3 -Og

debug: clean kernel8.img
	@mkdir $(DEBUG_DIR)
	$(OBJ_DUMP) -d $(ELF_ELE) > $(DEBUG_DIR)/asm
	$(OBJ_DUMP) -t $(ELF_ELE)  > $(DEBUG_DIR)/symbol
	$(EMULATOR) -M raspi3 -kernel kernel8.img -serial stdio -s -S