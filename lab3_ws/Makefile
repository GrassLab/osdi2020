CC = aarch64-elf-gcc
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

all: clean target kernel8.img

kernel8.img: target/start.o target/syscall.o target/main.o target/buildin.o target/console.o target/uart.o target/timer.o target/printk.o target/vsprintf.o target/mbox.o target/framebuffer.o target/sys.o target/exception.o
	aarch64-elf-ld -nostdlib -nostartfiles $^ -I include  -T link.ld -o kernel8.elf
	aarch64-elf-objcopy -O binary kernel8.elf kernel8.img

target:
	mkdir -p target
target/start.o: init/start.S
	aarch64-elf-gcc $(CFLAGS) -c $^ -o $@

target/syscall.o: kernel/syscall.S
	aarch64-elf-gcc $(CFLAGS) -c $^ -o $@

target/main.o: init/main.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I include

target/buildin.o: kernel/buildin.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I include

target/console.o: kernel/console.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I include

target/uart.o: kernel/uart0.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I include

target/timer.o: kernel/timer.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I includej

target/printk.o: kernel/printk.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I include

target/vsprintf.o: kernel/vsprintf.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I include

target/mbox.o: kernel/mbox.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I include

target/framebuffer.o: kernel/framebuffer.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I include

target/sys.o: kernel/sys.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I include

target/exception.o: kernel/exception.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I include

clean:
	rm kernel8.elf target/*.o >/dev/null 2>/dev/null || true

debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial stdio -S -s
run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio
monitor:
	sudo screen /dev/ttyUSB0 115200
