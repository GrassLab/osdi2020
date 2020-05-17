CFLAGS = -ffreestanding -nostartfiles -nostdlib -g
INCS = -I .

ASM_FILES = $(shell find kernel -name "*.S")
C_FILES = $(shell find kernel -name "*.c")
OBJS = $(ASM_FILES:%.S=%_s.o) $(C_FILES:%.c=%_c.o)

.PHONY: all clean

all: kernel8.img

kernel/%_c.o: kernel/%.c
	aarch64-linux-gnu-gcc $(INCS) $(CFLAGS) -c -o $@ $<

kernel/%_s.o: kernel/%.S
	aarch64-linux-gnu-gcc $(INCS) -c -o $@ $<

kernel8.img: $(OBJS)
	aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf $^
	aarch64-linux-gnu-objcopy -O binary kernel8.elf $@

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial pty

debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s -serial null -serial stdio

clean:
	@rm -rf $(OBJS) kernel8.*
