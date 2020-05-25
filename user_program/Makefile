CC       = aarch64-none-linux-gnu-gcc
LD       = aarch64-none-linux-gnu-ld
OBJCOPY  = aarch64-none-linux-gnu-objcopy
CFLAGS   = -Wall -O0 -ffreestanding -nostdinc -nostdlib -nostartfiles -fno-zero-initialized-in-bss -g -MMD
INCLUDES = -I../include

LSCRIPT  = linker.ld
KERNEL   = user

all: $(KERNEL).lb

$(KERNEL).lb: $(KERNEL).img
	$(LD) -r -b binary $< -o $@

$(KERNEL).img: $(KERNEL).elf
	$(OBJCOPY) -O binary $< $@

$(KERNEL).elf: $(KERNEL).o ../sysCall.o userLib.o
	$(LD) -T $(LSCRIPT) -o $@ $^

userLib.o: userLib.S
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(KERNEL).o: userProgram.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) *.o *.d $(KERNEL).elf $(KERNEL).img $(KERNEL).lb