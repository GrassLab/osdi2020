CC       = aarch64-none-linux-gnu-gcc
LD       = aarch64-none-linux-gnu-ld
OBJCOPY  = aarch64-none-linux-gnu-objcopy
EMULATOR = qemu-system-aarch64
CFLAGS   = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -g
INCLUDES = -Iinclude

SRCDIR	 = src
SRC		 = $(wildcard $(SRCDIR)/*.c)
ASMSRC	 = $(wildcard $(SRCDIR)/*.S)
OBJS	 = $(patsubst $(SRCDIR)/%.c,%.o,$(SRC)) \
		   $(patsubst $(SRCDIR)/%.S,%.o,$(ASMSRC))

LSCRIPT  = linker.ld
KERNEL   = kernel8

VPATH    = src

all: $(KERNEL).img

$(KERNEL).img: $(KERNEL).elf
	$(OBJCOPY) -O binary $< $@

$(KERNEL).elf: start.o $(OBJS)
	$(LD) -T $(LSCRIPT) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean test debug monitor

bl:
	make -C boot_loader/

bl-clean:
	make clean -C boot_loader/

bl-test:
	make test -C boot_loader/

bl-debug:
	make debug -C boot_loader/

clean:
	$(RM) *.o $(KERNEL).elf $(KERNEL).img

test: $(KERNEL).img
	$(EMULATOR) -M raspi3 -kernel $< -display none -serial stdio

debug: $(KERNEL).img
	$(EMULATOR) -M raspi3 -kernel $< -display none -serial stdio -S -s

monitor:
	screen /dev/pts/23 115200
	