A_SRCS = $(wildcard src/*.S)
C_SRCS = $(wildcard src/*.c) 
OBJS = $(A_SRCS:.S=.o) $(C_SRCS:.c=.o)

.PHONY: clean

all: clean kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

kernel8.elf: $(OBJS)
	aarch64-linux-gnu-ld $(OBJS) -T src/linker.ld -o kernel8.elf

src/%.o: src/%.S
	aarch64-linux-gnu-gcc -c $< -nostdlib -o $@

src/%.o: src/%.c
	aarch64-linux-gnu-gcc -c $< -nostdlib -o $@

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio

clean:
	rm -f $(OBJS) *.elf *.img
