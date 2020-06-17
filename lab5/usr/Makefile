C_FILES = $(wildcard *.c)
ASM_FILES = $(wildcard *.S)
OBJS =  $(ASM_FILES:.S=.o)
OBJS += $(C_FILES:.c=.o)
CFLAGS = -Wall -ffreestanding -nostdinc  -nostartfiles -ggdb -Og -fomit-frame-pointer

all:clean build
clean:
	rm *.elf *.img *.o >/dev/null 2>/dev/null || true
%.o: %.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@ -O0
%.o: %.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@ -O0
build: $(OBJS)
	aarch64-linux-gnu-ld -T link.ld -o usr.elf $(OBJS)
	aarch64-linux-gnu-objcopy  -O binary usr.elf usr.img
	aarch64-linux-gnu-ld  -r -b binary usr.img -o rd
	mv rd ../