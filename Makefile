CC       = aarch64-linux-gnu-gcc
LD       = aarch64-linux-gnu-ld
OBJCOPY  = aarch64-linux-gnu-objcopy
QEMU     = qemu-system-aarch64

LKSCRIPT = linker.ld
CFLAGS   = -g

TAR      = kernel8
SRC      = kernel8
ASM      = ${SRC:%=%.s}
OBJ      = ${SRC:%=%.o}
ELF      = $(TAR).elf
IMG      = $(TAR).img

all: $(IMG)

$(OBJ): $(ASM)
	$(CC) $(CFLAGS) -c $(ASM)

$(ELF): $(OBJ)
	$(LD) -T $(LKSCRIPT) -o $(ELF) $(OBJ)

$(IMG): $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(IMG)

test: $(IMG)
	$(QEMU) -M raspi3 -kernel $(IMG) -display none -d in_asm

gdb: $(IMG)
	$(QEMU) -M raspi3 -kernel $(IMG) -display none -S -s

clean:
	rm -rf  $(ELF) $(IMG) $(OBJ)

# alias
obj: $(OBJ)
elf: $(ELF)
img: $(IMG)
