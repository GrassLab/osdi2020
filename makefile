
ARMGNU = aarch64-linux-gnu-
CC = $(ARMGNU)gcc
LDFLAGS = -T linker.ld -nostdlib
SDCARD ?= /dev/sdb
HEADER := $(wildcard *.h)
SRC := $(wildcard *.c)
OBJECTS := $(patsubst %.c,%.o,$(SRC))
ASM := $(wildcard *.S)
CFLAGS = -fpie -pie -fno-stack-protector -nostdlib -nostartfiles -ffreestanding

.PHONY: all clean qemu debug indent

all: kernel8.img

$(wildcard */*.o): $(SRC) $(HEADER)

kernel8.elf: $(ASM) $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(ASM) $(OBJECTS)

kernel8.img: kernel8.elf
	$(ARMGNU)objcopy -O binary kernel8.elf kernel8.img

send_kernel:
	python load_images.py --port "/dev/ttyUSB0" --kernel "other_kernels/kernel8.img"

clean:
	rm -f kernel8.elf kernel8.img $(patsubst %,%~*,$(SRC) $(HEADER)) $(OBJECTS)


run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio -display none

run-detail:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio

run-mini-uart:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial pty

run-uart0:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial pty

run-script:
	sudo python script.py

connect:
	sudo screen /dev/ttyUSB0 115200

check-elf:
	aarch64-linux-gnu-readelf -s kernel8.elf
