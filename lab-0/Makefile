CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
TARGET = kernel8.img

$(TARGET):
	$(CC) -g -c a.S
	$(LD) -T linker.ld -o kernel8.elf a.o
	$(OBJCOPY) -O binary kernel8.elf $@

clean: 
	rm a.o $(TARGET) kernel8.elf >/dev/null 2>/dev/null
