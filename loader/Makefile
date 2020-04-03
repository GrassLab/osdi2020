CC			= aarch64-linux-gnu-gcc
LD 			= aarch64-linux-gnu-ld
OBJ_CPY 	= aarch64-linux-gnu-objcopy
EMULATOR	= qemu-system-aarch64

CFLAGS 		= -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

SRCDIR 		= src

SRCS 		= $(wildcard $(SRCDIR)/*.c )
OBJS 		= $(patsubst $(SRCDIR)/%.c, %.o, $(SRCS))
LINK_SCRIPT = src/link.ld

VPATH = $(SRCDIR)
vpath %.c %.S $(SRCDIR) 

all: clean kernel8.img

clean:
	@rm kernel8.elf kernel8.img *.o >/dev/null 2>/dev/null || true

kernel8.img: start.o $(OBJS)
	$(LD) -nostdlib -nostartfiles $^ -T $(LINK_SCRIPT) -o kernel8.elf
	$(OBJ_CPY) -O binary kernel8.elf $@

start.o: start.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: kernel8.img
	$(EMULATOR) -M raspi3 -kernel kernel8.img -serial stdio