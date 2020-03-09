CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJDUMP = aarch64-linux-gnu-objcopy
QEMU = qemu-system-aarch64

CFLAGS = -Wall -g -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
LDFLAGS = -nostdlib -nostartfiles

LINKER_SRC = link.ld
ELF_TARGET = kernel8.elf
TARGET = kernel8.img

all: clean $(TARGET)

boot.o: boot.S
	$(CC) $(CFLAGS) -c $^ -o $@

$(TARGET): boot.o
	$(LD) $(LDFLAGS) $< -T $(LINKER_SRC) -o $(ELF_TARGET)
	$(OBJDUMP) -O binary $(ELF_TARGET) $(TARGET)

clean:
	$(RM) $(TARGET) $(ELF_TARGET) *.o >/dev/null 2>& 1 || true

asmrun: $(TARGET)
	$(QEMU) -M raspi3 -kernel $< -display none -d in_asm

debugrun: $(TARGET)
	$(QEMU) -M raspi3 -kernel $< -display none -S -s &
