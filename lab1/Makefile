CC = aarch64-elf-gcc
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

all: clean target kernel8.img

kernel8.img: target/start.o target/main.o target/buildin.o target/console.o target/uart.o target/timer.o target/printk.o target/vsprintf.o
	aarch64-elf-ld -nostdlib -nostartfiles $^ -I include  -T link.ld -o kernel8.elf
	aarch64-elf-objcopy -O binary kernel8.elf kernel8.img

target:
	mkdir -p target
target/start.o: init/start.S
	aarch64-elf-gcc $(CFLAGS) -c init/start.S -o target/start.o

target/main.o: init/main.c
	$(CC) $(CFLAGS) -c init/main.c -o target/main.o -I include

target/buildin.o: kernel/buildin.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I include

target/console.o: kernel/console.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I include

target/uart.o: kernel/uart.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I include

target/timer.o: kernel/timer.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I includej

target/printk.o: kernel/printk.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I include

target/vsprintf.o: kernel/vsprintf.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I include

clean:
	rm kernel8.elf target/*.o >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio
monitor:
	sudo screen /dev/ttyUSB0 115200
