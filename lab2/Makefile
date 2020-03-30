ARMGNU = aarch64-linux-gnu-
CC = $(ARMGNU)gcc
LDFLAGS = -T linker.ld -nostdlib
SDCARD ?= /dev/sdb
SRC := $(wildcard *.c)
OBJECTS := $(patsubst %.c,%.o,$(SRC))
CFLAGS = -fno-stack-protector -include stackguard.h

.PHONY: all clean qemu debug indent

all: kernel8.img

$(wildcard *.o): $(wildcard *.h)

boot: $(OBJECTS)

kernel8.img: boot
	$(ARMGNU)objcopy -O binary boot kernel8.img

clean:
	rm boot kernel8.img *.o *.*~

qemu: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial pty

debug: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial pty -s -S

sd: kernel8.img
	sudo mount -t vfat $(SDCARD)1 rootfs/
	sudo cp kernel8.img rootfs/kernel8.img
	sudo umount rootfs

indent:
	indent *.c *.h
