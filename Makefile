ARM_GNU = aarch64-linux-gnu
CFLAGS = -Wall -Werror -g -nostdlib -nostartfiles -ffreestanding -Iinc -mgeneral-regs-only -D__DEBUG
# -Wextra 
S_SRCS = $(wildcard src/*.S)
C_SRCS = $(wildcard src/*.c)
S_LSRCS = $(wildcard lib/*.S)
C_LSRCS = $(wildcard lib/*.c)
S_OBJS = $(S_SRCS:src/%.S=build/%_s.o) $(S_LSRCS:lib/%.S=build/%_s.o)
C_OBJS = $(C_SRCS:src/%.c=build/%_c.o) $(C_LSRCS:lib/%.c=build/%_c.o)

all: kernel8.img

build/%_s.o: src/%.S
	$(ARM_GNU)-gcc $(CFLAGS) -c $< -o $@

build/%_c.o: src/%.c
	$(ARM_GNU)-gcc $(CFLAGS) -c $< -o $@

build/%_s.o: lib/%.S
	$(ARM_GNU)-gcc $(CFLAGS) -c $< -o $@

build/%_c.o: lib/%.c
	$(ARM_GNU)-gcc $(CFLAGS) -c $< -o $@

kernel8.img: $(S_OBJS) $(C_OBJS)
	$(ARM_GNU)-ld -T link.ld -o kernel8.elf $(S_OBJS) $(C_OBJS)
	$(ARM_GNU)-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.elf build/* >/dev/null 2>/dev/null || true

.PHONY: run
run: kernel8.img
	@qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial stdio

.PHONY: debug
debug: kernel8.img
	@qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial stdio -d int -s -S