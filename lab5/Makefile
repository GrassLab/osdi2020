C_FILES = $(wildcard *.c)
ASM_FILES = $(wildcard *.S)
OBJS =  $(ASM_FILES:.S=.o)
OBJS += $(C_FILES:.c=.o)
CFLAGS = -Wall -ffreestanding -nostdinc  -nostartfiles -ggdb -Og -fomit-frame-pointer

all: clean kernel8.img

clean:
	rm kernel8.* *.o >/dev/null 2>/dev/null || true
%.o: %.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@ -O0

%.o: %.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@ -O0



kernel8.img: $(OBJS)
	aarch64-linux-gnu-ld  $(OBJS) rd -T link.ld -o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img


debug: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial pty -s -S

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img  -serial stdio -display none

demo:
	sudo screen /dev/ttyUSB0 115200
