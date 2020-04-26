CC       = aarch64-none-linux-gnu-gcc
LD       = aarch64-none-linux-gnu-ld
OBJCOPY  = aarch64-none-linux-gnu-objcopy
EMULATOR = qemu-system-aarch64
CFLAGS   = -Wall -O0 -ffreestanding -nostdinc -nostdlib -nostartfiles -g
INCLUDES = -Iinclude

SRCDIR	 = src
SRC		 = $(wildcard $(SRCDIR)/*.c)
ASMSRC	 = $(wildcard $(SRCDIR)/*.S)
SRCOBJS	 = $(patsubst $(SRCDIR)/%.c,%.o,$(SRC)) \
		   $(patsubst $(SRCDIR)/%.S,%.o,$(ASMSRC))
IRQDIR	 = src/interrupt
IRQ		 = $(wildcard $(IRQDIR)/*.c)
ASMIRQ	 = $(wildcard $(IRQDIR)/*.S)
IRQOBJS	 = $(patsubst $(IRQDIR)/%.c,%.o,$(IRQ)) \
		   $(patsubst $(IRQDIR)/%.S,%.o,$(ASMIRQ))
DVDIR	 = src/device
DV		 = $(wildcard $(DVDIR)/*.c)
DVOBJS	 = $(patsubst $(DVDIR)/%.c,%.o,$(DV)) \
		   $(patsubst $(DVDIR)/%.S,%.o,$(ASMDV))
SCDDIR	 = src/schedule
SCD		 = $(wildcard $(SCDDIR)/*.c)
ASMSCD	 = $(wildcard $(SCDDIR)/*.S)
SCDOBJS	 = $(patsubst $(SCDDIR)/%.c,%.o,$(SCD)) \
		   $(patsubst $(SCDDIR)/%.S,%.o,$(ASMSCD))

OBJS = $(SRCOBJS) $(IRQOBJS) $(DVOBJS) $(SCDOBJS)

LSCRIPT  = linker.ld
KERNEL   = kernel8

VPATH    = $(SRCDIR) $(IRQDIR) $(DVDIR) $(SCDDIR)

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
	