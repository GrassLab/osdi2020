SRCS = $(wildcard *.c)
ASMF = $(wildcard *.S)
OBJS = $(SRCS:.c=.o)
AOBJ = $(ASMF:.S=.o)
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles




all: clean kernel8.img

%.o: %.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

%.o: %.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

kernel8.img: $(OBJS) $(AOBJ)
	aarch64-linux-gnu-ld  -nostartfiles $(OBJS) $(AOBJ) -T link.ld -o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img
clean:
	rm kernel8.elf *.o >/dev/null 2>/dev/null || true
asm:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -d in_asm
run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial pty
show:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio
debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s -serial pty

