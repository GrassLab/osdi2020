CC       = aarch64-linux-gnu-gcc
LD       = aarch64-linux-gnu-ld
OBJCOPY  = aarch64-linux-gnu-objcopy
EMULATOR = qemu-system-aarch64
CFLAGS   = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -g -MMD
INCLUDES = -Iinclude

SRCDIR	 = src
SRC		 = $(wildcard $(SRCDIR)/*.c)
ASMSRC	 = $(wildcard $(SRCDIR)/*.S)
SRCOBJS	 = $(patsubst $(SRCDIR)/%.c,%_c.o,$(SRC)) \
		   $(patsubst $(SRCDIR)/%.S,%.o,$(ASMSRC))
IRQDIR	 = src/interrupt
IRQ		 = $(wildcard $(IRQDIR)/*.c)
ASMIRQ	 = $(wildcard $(IRQDIR)/*.S)
IRQOBJS	 = $(patsubst $(IRQDIR)/%.c,%_c.o,$(IRQ)) \
		   $(patsubst $(IRQDIR)/%.S,%.o,$(ASMIRQ))
DVDIR	 = src/device
DV		 = $(wildcard $(DVDIR)/*.c)
DVOBJS	 = $(patsubst $(DVDIR)/%.c,%_c.o,$(DV)) \
		   $(patsubst $(DVDIR)/%.S,%.o,$(ASMDV))
TASKDIR	 = src/task
TASK     = $(wildcard $(TASKDIR)/*.c)
ASMTASK	 = $(wildcard $(TASKDIR)/*.S)
TASKOBJS = $(patsubst $(TASKDIR)/%.c,%_c.o,$(TASK)) \
		   $(patsubst $(TASKDIR)/%.S,%.o,$(ASMTASK))
MMDIR	 = src/memory
MM       = $(wildcard $(MMDIR)/*.c)
ASMMM    = $(wildcard $(MMDIR)/*.S)
MMOBJS   = $(patsubst $(MMDIR)/%.c,%_c.o,$(MM)) \
		   $(patsubst $(MMDIR)/%.S,%.o,$(ASMMM))

OBJS = $(SRCOBJS) $(IRQOBJS) $(DVOBJS) $(TASKOBJS) $(MMOBJS)

LSCRIPT  = linker.ld
KERNEL   = kernel8

VPATH    = $(SRCDIR) $(IRQDIR) $(DVDIR) $(TASKDIR) $(MMDIR)

all: $(KERNEL).img

$(KERNEL).img: $(KERNEL).elf
	$(OBJCOPY) -O binary $< $@

$(KERNEL).elf: start.o $(OBJS)
	$(LD) -T $(LSCRIPT) -o $@ $^

%_c.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.S
	$(CC) $(INCLUDES) -c $< -o $@

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
	$(RM) *.o *.d $(KERNEL).elf $(KERNEL).img

test: $(KERNEL).img
	$(EMULATOR) -M raspi3 -kernel $< -display none -serial stdio

debug: $(KERNEL).img
	$(EMULATOR) -M raspi3 -kernel $< -display none -serial stdio -S -s

monitor:
	screen /dev/pts/23 115200
	