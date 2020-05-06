CFLAGS = -ffreestanding -nostartfiles -nostdlib -g
INCS = -I kernel/include -I lib/include

ASM_FILES = $(shell find kernel lib -name "*.S")
C_FILES = $(shell find kernel lib -name "*.c")
OBJS = $(ASM_FILES:%.S=%_s.o) $(C_FILES:%.c=%_c.o)

.PHONY: all clean

all: kernel8.img

%_c.o: %.c
	aarch64-linux-gnu-gcc $(INCS) $(CFLAGS) -c -o $@ $<

%_s.o: %.S
	aarch64-linux-gnu-gcc -c -o $@ $<

kernel8.img: $(OBJS)
	aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf $^
	aarch64-linux-gnu-objcopy -O binary kernel8.elf $@

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial pty

debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s -serial null -serial stdio

clean:
	@rm -rf $(OBJS) kernel8.*
