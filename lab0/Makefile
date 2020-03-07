CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
LD_FLAGS = -T linker.ld

kerenl8.img: a.S
	$(CC) -c a.S
	$(LD) $(LD_FLAGS) -o kernel8.elf a.o
	$(OBJCOPY) -O binary kernel8.elf kerenl8.img
