CC       = aarch64-none-linux-gnu-gcc
LD       = aarch64-none-linux-gnu-ld
OBJCOPY  = aarch64-none-linux-gnu-objcopy
EMULATOR = qemu-system-aarch64
CFLAGS   = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
INCLUDES = -Iinclude

SRCDIR	 = src
SRC		 = $(wildcard $(SRCDIR)/*.c)
OBJS	 = $(patsubst $(SRCDIR)/%.c,%.o,$(SRC))

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

start.o: start.S
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean test

clean:
	$(RM) *.o $(KERNEL).elf $(KERNEL).img

test: $(KERNEL).img
	$(EMULATOR) -M raspi3 -kernel $< -display none -serial null -serial stdio