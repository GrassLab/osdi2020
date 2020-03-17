CC			:= aarch64-linux-gnu-gcc
LD			:= aarch64-linux-gnu-ld
OBJCOPY := aarch64-linux-gnu-objcopy

.PHONY: all run clean
all: kernel8.img

a.o: a.S
	$(CC) -c $<

%.o: %.c
	$(CC) -static -c $< -o $@

kernel8.elf: a.o main.o uart.o
	$(LD) -static -T linker.ld -o $@ $^

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary $< $@

run: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel $< -serial null -serial stdio -display none

clean:
	rm -rf *.o kernel8.*
