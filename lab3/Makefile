CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy

GENERAL_FLAGS =
C_FLAGS = -Wall -Wextra -Wshadow -Wconversion -nostdlib -nostartfiles

KERNEL_C_SOURCE_FILES = main.c shell.c string_util.c mailbox.c uart.c exc.c
KERNEL_C_OBJECT_FILES = $(KERNEL_C_SOURCE_FILES:.c=.o)
KERNEL_ASSEMBLY_SOURCE_FILES = start.s get_time.s exc_vector.s
KERNEL_ASSEMBLY_OBJECT_FILES = $(KERNEL_ASSEMBLY_SOURCE_FILES:.s=.o)

all: kernel8.img

kernel8.img: kernel8.elf
	${OBJCOPY} -O binary kernel8.elf kernel8.img

kernel8.elf: ${KERNEL_ASSEMBLY_OBJECT_FILES} ${KERNEL_C_OBJECT_FILES}
	${LD} -T kernel.ld ${KERNEL_ASSEMBLY_OBJECT_FILES} ${KERNEL_C_OBJECT_FILES} -o kernel8.elf

%.o: %.c
	${CC} -c ${GENERAL_FLAGS} ${C_FLAGS} $^ -o $@

%.o: %.s
	${CC} -c ${GENERAL_FLAGS} $^ -o $@

debug: GENERAL_FLAGS += -DDEBUG -g3
debug: all

qemu: all
	@echo "Make sure .text in os.ld is at 0x80000"
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -vnc :0 -serial stdio

qemu_dbg: debug
	@echo "Make sure .text in os.ld is at 0x80000"
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -vnc :0 -s -S -serial pty

.PHONY: clean
clean:
	rm -rf *.o *.elf *img

