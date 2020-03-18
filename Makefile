CFLAGS = -ffreestanding -nostartfiles -nostdlib

ASM_FILES = $(wildcard src/*.S)
C_FILES = $(wildcard src/*.c)
OBJS = $(ASM_FILES:.S=.o)
OBJS += $(C_FILES:.c=.o)

.PHONY: all clean

all: kernel8.img

src/%.o: src/%.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c -o $@ $<

src/%.o: src/%.S
	aarch64-linux-gnu-gcc -c -o $@ $<

kernel8.img: $(OBJS)
	aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf $^
	aarch64-linux-gnu-objcopy -O binary kernel8.elf $@

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio

clean:
	@rm src/*.o kernel8.*
