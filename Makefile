CC       = aarch64-none-linux-gnu-gcc
LD       = aarch64-none-linux-gnu-ld
OBJCOPY  = aarch64-none-linux-gnu-objcopy
EMULATOR = qemu-system-aarch64

LSCRIPT  = linker.ld
KERNEL   = kernel8

VPATH    = src

all: $(KERNEL).img

$(KERNEL).img: $(KERNEL).elf
	$(OBJCOPY) -O binary $< $@

$(KERNEL).elf: $(KERNEL).o
	$(LD) -T $(LSCRIPT) -o $@ $<

%.o: %.S
	$(CC) -c $<

.PHONY: clean test

clean:
	$(RM) $(KERNEL).o $(KERNEL).elf $(KERNEL).img

test: $(KERNEL).img
	$(EMULATOR) -M raspi3 -kernel $< -display none -d in_asm