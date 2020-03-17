CC = aarch64-linux-gnu-gcc
AS = aarch64-linux-gnu-as
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
CC_FLAGS = -nostdlib -fno-builtin
LD_FLAGS = -T linker.ld -nostdlib 

all: kernel8.img


startup.o: startup.s
	$(AS) -c $< -o $@

%.o : %.c
	$(CC) $(CC_FLAGS) -c $< -o $@

kernel8.img: shell.o mystd.o startup.o uart.o bcm2835_wdt.o linker.ld
	$(LD) $(LD_FLAGS) -o kernel8.elf startup.o shell.o uart.o mystd.o bcm2835_wdt.o
	$(OBJCOPY) -O binary kernel8.elf $@

clean:
	rm *.o

debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm -serial null -serial stdio

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null  -serial stdio
