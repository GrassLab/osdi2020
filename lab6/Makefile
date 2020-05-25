CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy

GENERAL_FLAGS =
C_FLAGS = -Wall -Wextra -Wshadow -Wconversion -nostdlib -nostartfiles -mstrict-align
ASM_FLAGS =

KERNEL_C_SOURCE_FILES = string_util.c mailbox.c uart.c exc.c irq.c syscall.c timer.c schedule.c task.c sys.c queue.c mmu.c
KERNEL_C_OBJECT_FILES = $(KERNEL_C_SOURCE_FILES:.c=.o)
KERNEL_ASSEMBLY_SOURCE_FILES = start.S exc_vector.S timer.S schedule.S mmu.S
KERNEL_ASSEMBLY_OBJECT_FILES = $(KERNEL_ASSEMBLY_SOURCE_FILES:.S=.asmo)

SHELL_C_SOURCE_FILES = shell.c string_util.c syscall.c c_routine.c
SHELL_C_OBJECT_FILES = $(SHELL_C_SOURCE_FILES:.c=.o)
TEST1_C_SOURCE_FILES = test1.c string_util.c syscall.c c_routine.c
TEST1_C_OBJECT_FILES = $(TEST1_C_SOURCE_FILES:.c=.o)
TEST2_C_SOURCE_FILES = test2.c string_util.c syscall.c c_routine.c
TEST2_C_OBJECT_FILES = $(TEST2_C_SOURCE_FILES:.c=.o)

all: kernel8.img

kernel8.img: kernel8.elf
	${OBJCOPY} -O binary kernel8.elf kernel8.img

kernel8.elf: ${KERNEL_ASSEMBLY_OBJECT_FILES} ${KERNEL_C_OBJECT_FILES} test1.rd test2.rd shell.rd
	${LD} -T kernel.ld $^ -o $@

test1.elf: ${TEST1_C_OBJECT_FILES}
	${LD} -T user.ld $^ -o $@

test2.elf: ${TEST2_C_OBJECT_FILES}
	${LD} -T user.ld $^ -o $@

shell.elf: ${SHELL_C_OBJECT_FILES}
	${LD} -T user.ld $^ -o $@

%.rd: %.bin
	${LD} -r -b binary $^ -o $@

%.bin: %.elf
	${OBJCOPY} $^ -O binary $@

%.o: %.c
	${CC} -c ${GENERAL_FLAGS} ${C_FLAGS} $^ -o $@

%.asmo: %.S
	${CC} -c ${GENERAL_FLAGS} ${ASM_FLAGS} $^ -o $@

debug: GENERAL_FLAGS += -DDEBUG -g3
debug: all

qemu: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -vnc :0 -serial stdio

copy: all
	sudo mount /dev/sdb1 /mnt/ && sudo cp kernel8.img /mnt/ && sudo umount /mnt

.PHONY: qemu_dbg
qemu_dbg: debug
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -vnc :0 -s -S -serial pty

.PHONY: clean
clean:
	rm -rf *.o *.elf *img *.asmo *.bin *.rd

