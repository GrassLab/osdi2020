ARMGNU = aarch64-linux-gnu
CFLAGS = -Wall -O2 -ffreestanding -fno-stack-protector -nostdinc -nostdlib -nostartfiles

all: clean kernel8.img

kernel/start.o: kernel/start.S
	$(ARMGNU)-gcc $(CFLAGS) -c kernel/start.S -o kernel/start.o

kernel/%.o: kernel/%.c
	$(ARMGNU)-gcc $(CFLAGS) -c $< -o $@

allocator/%.o: allocator/%.c
	$(ARMGNU)-gcc $(CFLAGS) -c $< -o $@

fs/%.o: fs/%.c
	$(ARMGNU)-gcc $(CFLAGS) -c $< -o $@

KERNEL = $(wildcard kernel/*.c)
ALLOCATOR = $(wildcard allocator/*.c)
FS = $(wildcard fs/*.c)
OBJS = $(KERNEL:kernel/%.c=kernel/%.o)
OBJS += $(ALLOCATOR:allocator/%.c=allocator/%.o)
OBJS += $(FS:fs/%.c=fs/%.o)

kernel8.img: kernel/start.o $(OBJS)
	$(ARMGNU)-ld -nostdlib -nostartfiles kernel/start.o $(OBJS) -T kernel/link.ld -o kernel8.elf
	$(ARMGNU)-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.elf kernel8.img $(OBJS) >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio

objdump:
	$(ARMGNU)-objdump -d kernel8.elf

readelf:
	$(ARMGNU)-readelf -a kernel8.elf
