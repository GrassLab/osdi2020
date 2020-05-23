CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
TARGET = kernel8.img
CFLAGS = -I include -g -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

SRCS = $(wildcard src/*.c)
ASMS = $(wildcard src/*.S)
OBJS = $(SRCS:.c=.o)

all: $(TARGET)
run: $(TARGET)
	qemu-system-aarch64 -M raspi3 -kernel $(TARGET) -serial stdio -display none -s
debug: $(TARGET)
	qemu-system-aarch64 -M raspi3 -kernel $(TARGET) -s -serial stdio -display none -d int -S
loader: 
	qemu-system-aarch64 -M raspi3 -kernel ./bin/kernel8.img -s -serial pty -display none

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(ASMS) $(OBJS)
	$(CC) -I include -g -nostdlib -nostartfiles $(ASMS) $(OBJS) -T src/linker.ld -o kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf $@

clean:
	rm $(OBJS) $(TARGET) kernel8.elf >/dev/null 2>/dev/null