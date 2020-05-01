CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy

GENERAL_FLAGS =
C_FLAGS = -Wall -Wextra -Wshadow -Wconversion -nostdlib -nostartfiles
ASM_FLAGS =

KERNEL_C_SOURCE_FILES = main.c shell.c string_util.c mailbox.c uart.c exc.c irq.c syscall.c timer.c schedule.c task.c
KERNEL_C_OBJECT_FILES = $(KERNEL_C_SOURCE_FILES:.c=.o)
KERNEL_ASSEMBLY_SOURCE_FILES = start.s exc_vector.s timer.s schedule.s
KERNEL_ASSEMBLY_OBJECT_FILES = $(KERNEL_ASSEMBLY_SOURCE_FILES:.s=.asmo)

all: kernel8.img

kernel8.img: kernel8.elf
	${OBJCOPY} -O binary kernel8.elf kernel8.img

kernel8.elf: ${KERNEL_ASSEMBLY_OBJECT_FILES} ${KERNEL_C_OBJECT_FILES}
	@echo "Make sure .text in os.ld is at 0x80000"
	${LD} -T kernel.ld ${KERNEL_ASSEMBLY_OBJECT_FILES} ${KERNEL_C_OBJECT_FILES} -o kernel8.elf

%.o: %.c
	${CC} -c ${GENERAL_FLAGS} ${C_FLAGS} $^ -o $@

%.asmo: %.s
	${CC} -c ${GENERAL_FLAGS} ${ASM_FLAGS} $^ -o $@

debug: GENERAL_FLAGS += -DDEBUG -g3
debug: all

qemu: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -vnc :0 -serial stdio

.PHONY: qemu_dbg
qemu_dbg: debug
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -vnc :0 -s -S -serial pty

.PHONY: clean
clean:
	rm -rf *.o *.elf *img *.asmo

