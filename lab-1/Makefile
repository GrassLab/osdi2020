CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
TARGET = kernel8.img
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)


all: $(TARGET)
run: $(TARGET)
	qemu-system-aarch64 -M raspi3 -kernel $(TARGET) -serial null -serial stdio
debug: $(TARGET)
	qemu-system-aarch64 -M raspi3 -kernel $(TARGET) -S -s
start.o: start.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): start.o $(OBJS)
	$(CC) -g $(CFLAGS) -c start.S -o start.o
	$(LD) -nostdlib -nostartfiles start.o $(OBJS) -T linker.ld -o kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf $@

%.o: %.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

clean: 
	rm *.o $(TARGET) kernel8.elf >/dev/null 2>/dev/null
