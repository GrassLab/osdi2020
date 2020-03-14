CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
QEMU = qemu-system-aarch64

SRC_DIR = src
BOOT_DIR = boot
LIB_DIR = lib
OBJ_DIR = obj
OUT_DIR = output

CFLAGS = -I$(LIB_DIR) -Wall -g -O2 -march=armv8-a -mtune=cortex-a53 -ffreestanding -nostdinc -nostdlib -nostartfiles
LDFLAGS = -nostdlib -nostartfiles

OBJ = $(notdir $(patsubst %.S, %.o, $(wildcard $(BOOT_DIR)/*.S)))
OBJ += $(notdir $(patsubst %.c, %.o, $(wildcard $(SRC_DIR)/*.c)))
LINKER_SRC = $(addprefix $(BOOT_DIR)/, link.ld)
ELF_TARGET = $(addprefix $(OUT_DIR)/, kernel8.elf)
TARGET = $(addprefix $(OUT_DIR)/, kernel8.img)

all: $(TARGET)

$(OBJ_DIR)/%.o : $(addprefix $(BOOT_DIR)/, %.S)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJ_DIR)/%.o: $(addprefix $(SRC_DIR)/, %.c)
	$(CC) $(CFLAGS) -c $^ -o $@

$(TARGET): $(addprefix $(OBJ_DIR)/, $(OBJ))
	$(LD) $(LDFLAGS) $^ -T $(LINKER_SRC) -o $(ELF_TARGET)
	$(OBJCOPY) -O binary $(ELF_TARGET) $(TARGET)

clean:
	$(RM) -r $(OBJ_DIR) $(OUT_DIR)

run: $(TARGET)
	$(QEMU) -M raspi3 -kernel $< -serial null -serial mon:stdio -nographic

asmrun: $(TARGET)
	$(QEMU) -M raspi3 -kernel $< -display none -d in_asm

debugrun: $(TARGET)
	$(QEMU) -M raspi3 -kernel $< -display none -S -s &

$(shell mkdir -p $(OBJ_DIR) $(OUT_DIR))
