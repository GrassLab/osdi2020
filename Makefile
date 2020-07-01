TOOLCHAIN_PREFIX = aarch64-linux-gnu-
GCC = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)ld
OBJCPY = $(TOOLCHAIN_PREFIX)objcopy
QEMU = qemu-system-aarch64

SRC_DIR = src
OUTPUT_DIR = output
LIB_DIR = include

LINKER_FILE = $(SRC_DIR)/linker.ld
SRCS = $(wildcard $(SRC_DIR)/*.c)
ASM = $(wildcard $(SRC_DIR)/*.S)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OUTPUT_DIR)/%_c.o)
ASM_OBJS = $(ASM:$(SRC_DIR)/%.S=$(OUTPUT_DIR)/%_s.o)
CFLAGS = -Wall -O2 -ffreestanding -nostartfiles -I$(LIB_DIR) -g

ELF = kernel8.elf
IMG = kernel8.img

all: clean makedir $(IMG)

$(OUTPUT_DIR)/%_s.o: $(SRC_DIR)/%.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

$(OUTPUT_DIR)/%_c.o: $(SRC_DIR)/%.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

$(IMG): $(ASM_OBJS) $(OBJS)
	$(LD) $(ASM_OBJS) $(OBJS) -T $(LINKER_FILE) -o $(ELF)
	$(OBJCPY) -O binary $(ELF) $(IMG)

clean:
	rm -f $(ELF) $(IMG) $(OUTPUT_DIR)/* 

makedir:
	mkdir -p $(OUTPUT_DIR)

run:
	$(QEMU) -M raspi3 -kernel $(IMG) -display none -serial stdio
tty:
	$(QEMU) -M raspi3 -kernel $(IMG) -display none -serial "pty"
debug:
	$(QEMU) -M raspi3 -kernel $(IMG) -display none -serial "pty" -S -s
sddebug:
	$(QEMU) -M raspi3 -kernel $(IMG) -display none -serial "pty" -S -s -drive if=sd,file=sfn_nctuos.img,format=raw
sd:
	$(QEMU) -M raspi3 -kernel $(IMG) -display none -serial stdio -drive if=sd,file=sfn_nctuos.img,format=raw
