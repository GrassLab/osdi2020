ARMGNU = aarch64-linux-gnu

CFLAGS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only

.PHONY: all clean run gdb objdump readelf copy

all: clean kernel8.img

src/%_c.o: src/%.c 
	$(ARMGNU)-gcc $(CFLAGS) -c $< -o $@

src/%_s.o: src/%.S 
	$(ARMGNU)-gcc  -g -c  $< -o $@

user/user.o: user/user.c
	 $(ARMGNU)-gcc $(COPS) -fno-zero-initialized-in-bss -g -c user/user.c -o user/user.o

user/lib.o: user/lib.S
	$(ARMGNU)-gcc $(COPS) -g -c user/lib.S -o user/lib.o

user_raw: user/user.o user/lib.o
	$(ARMGNU)-ld -T user/linker.ld -o user.elf user/user.o user/lib.o
	$(ARMGNU)-objcopy user.elf -O binary user.img
	$(ARMGNU)-ld -r -b binary user.img -o user_raw

SRCS = $(wildcard src/*.c)
ASMS = $(wildcard src/*.S)
OBJS = $(SRCS:src/%.c=src/%_c.o)
OBJS += $(ASMS:src/%.S=src/%_s.o)


kernel8.elf: $(OBJS) src/link.ld user_raw
	$(ARMGNU)-ld $(OBJS) user_raw -T src/link.ld -o kernel8.elf 

kernel8.img: kernel8.elf
	$(ARMGNU)-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.img kernel8.elf src/*.o >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio

gdb:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial stdio -S -s

objdump:
	$(ARMGNU)-objdump -d kernel8.elf

readelf:
	$(ARMGNU)-readelf -a kernel8.elf
	
copy:
	cp kernel8.img /media/hank0438/4DFF-0A36/