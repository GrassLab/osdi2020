TOOLCHAIN_PREFIX = aarch64-linux-gnu-
GCC = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)ld
OBJCPY = $(TOOLCHAIN_PREFIX)objcopy
QEMU = qemu-system-aarch64

SRC_DIR = src
OUTPUT_DIR = output
LIB_DIR = include

LINKER_FILE = $(SRC_DIR)/linker.ld
ENTRY = $(SRC_DIR)/start.s
ENTRY_OBJS = $(OUTPUT_DIR)/start.o
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OUTPUT_DIR)/%.o)
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -I$(LIB_DIR)

ELF = kernel8.elf
IMG = kernel8.img

all: clean makedir $(IMG)

$(ENTRY_OBJS): $(ENTRY)
	aarch64-linux-gnu-gcc $(CFLAGS) -c $(ENTRY) -o $(ENTRY_OBJS) 

$(OUTPUT_DIR)/%.o: $(SRC_DIR)/%.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

$(IMG): $(ENTRY_OBJS) $(OBJS)
	$(LD) $(ENTRY_OBJS) $(OBJS) -T $(LINKER_FILE) -o $(ELF)
	$(OBJCPY) -O binary $(ELF) $(IMG)

clean:
	rm -f $(ELF) $(IMG) $(OUTPUT_DIR)/* 

makedir:
	mkdir -p $(OUTPUT_DIR)

run:
	$(QEMU) -M raspi3 -kernel $(IMG) -serial stdio
