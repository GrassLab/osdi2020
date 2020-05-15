ARMGNU = aarch64-linux-gnu-
CC = $(ARMGNU)gcc
LDFLAGS = -T kernel/linker.ld -nostdlib
SDCARD ?= /dev/sdb
HEADER := $(wildcard */*.h)
SRC := $(wildcard */*.c)
ASM := $(wildcard asm/*/*.S)
OBJECTS := $(patsubst %.S,%.o,$(ASM)) $(patsubst %.c,%.o,$(SRC))
CFLAGS = -include include/stackguard.h -Iinclude -Ilib -Iperipheral -Isched -Ikernel -Imm -Wextra
ASFLAGS =

.PHONY: all clean qemu debug indent

all: kernel8.img

$(wildcard */*.o): $(SRC) $(HEADER) $(ASM)

kernel8.elf: $(OBJECTS) user_shell
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) user/bin_shell.o

kernel8.img: kernel8.elf
	$(ARMGNU)objcopy -O binary kernel8.elf kernel8.img

clean:
	rm -f kernel8.elf kernel8.img $(patsubst %,%~*,$(SRC) $(HEADER)) $(OBJECTS)

qemu: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio

debug: CFLAGS += -ggdb -Og

debug: ASFLAGS += -ggdb -Og

debug: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio -s -S

sd: kernel8.img
	sudo mount -t vfat $(SDCARD)1 rootfs/
	sudo cp kernel8.img rootfs/kernel8.img
	sudo umount rootfs

indent:
	indent $(SRC) $(HEADER) -Tsize_t

user_shell:
	make -C user/
