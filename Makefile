CC			= aarch64-linux-gnu-gcc
LD 			= aarch64-linux-gnu-ld
OBJ_CPY 	= aarch64-linux-gnu-objcopy
OBJ_DUMP	= aarch64-linux-gnu-objdump
EMULATOR	= qemu-system-aarch64

CFLAGS 		= -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
INCLUDE_FLG = -Iinclude -I.

SRCS 		= $(wildcard kernel/*/*.c) $(wildcard lib/*.c) $(wildcard src/*.c)
ASMS		= $(wildcard kernel/*/*.S) $(wildcard kernel/*.S)
SRC_OBJS 	= $(SRCS:%.c=%.o)
ASM_OBJS	= $(ASMS:%.S=%.S.o)

ALL_OBJS	= $(SRC_OBJS) $(ASM_OBJS)
LINK_SCRIPT = link.ld

.PHONY: all clean run force

all: kernel8.img
force: clean kernel8.img

clean:
	@rm asm symbol kernel8.elf $(ALL_OBJS)  >/dev/null 2>/dev/null || true

kernel8.img: kernel8.elf
	$(OBJ_CPY) -O binary $^ $@

kernel8.elf: $(ALL_OBJS)
	$(LD) -nostdlib -nostartfiles $^ -T $(LINK_SCRIPT) -o kernel8.elf

%.S.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE_FLG) -c $< -o $@

run: kernel8.img
	$(EMULATOR) -M raspi3 -kernel kernel8.img -serial stdio

debug: CFLAGS += -ggdb3 -Og

debug: clean kernel8.img
	$(OBJ_DUMP) -d kernel8.elf > asm
	$(OBJ_DUMP) -t kernel8.elf > symbol
	$(EMULATOR) -M raspi3 -kernel kernel8.img -serial stdio -s -S