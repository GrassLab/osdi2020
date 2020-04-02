TOOLCHAIN_PREFIX = aarch64-linux-gnu
CFLAGS = -Wall -g -nostdlib -nostartfiles -ffreestanding -Iinc

S_SRCS = $(wildcard src/*.S)
C_SRCS = $(wildcard src/*.c)
S_OBJS = $(S_SRCS:src/%.S=build/%_s.o)
C_OBJS = $(C_SRCS:src/%.c=build/%_c.o)

.PHONY: all clean run debug

all:clean kernel8.img

build/%_s.o: src/%.S
	$(TOOLCHAIN_PREFIX)-gcc $(CFLAGS) -c $< -o $@

build/%_c.o: src/%.c
	$(TOOLCHAIN_PREFIX)-gcc $(CFLAGS) -c $< -o $@

kernel8.img: $(S_OBJS) $(C_OBJS)
	$(TOOLCHAIN_PREFIX)-ld -T link.ld -o kernel8.elf $(S_OBJS) $(C_OBJS)
	$(TOOLCHAIN_PREFIX)-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.elf build/* >/dev/null 2>/dev/null || true

run:
	@qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial pty

debug:
	@qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial pty -s -S