SRC:=$(wildcard *.c)
HDR:=$(wildcard *.h)
OBJ:=$(patsubst %.c,%.o,$(SRC))

all: kernel8.img

handlers.o: handlers.S
	aarch64-linux-gnu-gcc -c handlers.S -o handlers.o

start.o: start.S
	aarch64-linux-gnu-gcc -c start.S -o start.o

exc_table.o: exc_table.S
	aarch64-linux-gnu-gcc -c exc_table.S -o exc_table.o

exc_contexts.o: exc_contexts.c
	aarch64-linux-gnu-gcc -c exc_contexts.c -o exc_contexts.o

main.o: main.c
	aarch64-linux-gnu-gcc -c main.c -o main.o

timer.o:timer.S
	aarch64-linux-gnu-gcc -c timer.S -o timer.o

mbox.o:mbox.c
	aarch64-linux-gnu-gcc -c mbox.c -o mbox.o

uart.o: uart.c
	aarch64-linux-gnu-gcc -c uart.c -o uart.o

utils.o: utils.c
	aarch64-linux-gnu-gcc -c utils.c -o utils.o

hard_info.o:hard_info.c
	aarch64-linux-gnu-gcc -c hard_info.c -o hard_info.o

kernel8.elf: start.o uart.o main.o utils.o hard_info.o mbox.o exc_table.o exc_contexts.o handlers.o timer.o
	aarch64-linux-gnu-ld -T link.ld -o kernel8.elf start.o uart.o main.o utils.o hard_info.o mbox.o exc_table.o handlers.o exc_contexts.o timer.o

#kernel8.elf: $(OBJ)
#	aarch64-linux-gnu-ld -T link.ld -o kernel8.elf $(OBJ)

kernel8.img: kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

clean: 
	rm kernel8.elf *.o ||true

run:	
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null

ser:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial pty
# -s -S -gdb tcp::1234


