CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy

GENERAL_FLAGS =
C_FLAGS = -Wall -Wextra -Wshadow -Wconversion -nostdlib -nostartfiles

KERNEL_C_SOURCE_FILES = main.c shell.c miniuart.c
KERNEL_C_OBJECT_FILES = $(KERNEL_C_SOURCE_FILES:.c=.o)

all: kernel8.img

kernel8.img: kernel8.elf
	${OBJCOPY} -O binary kernel8.elf kernel8.img

kernel8.elf: start.o ${KERNEL_C_OBJECT_FILES}
	${LD} -T linker.ld start.o ${KERNEL_C_OBJECT_FILES} -o kernel8.elf

start.o: start.s
	${CC} -c ${GENERAL_FLAGS} start.s -o start.o

%.o: %.c
	${CC} -c ${GENERAL_FLAGS} ${C_FLAGS} $^ -o $@

debug: GENERAL_FLAGS += -DDEBUG -g3
debug: all

qemu: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio

qemu_dbg: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -s -S

.PHONY: clean
clean:
	rm -rf *.o *.elf *img

