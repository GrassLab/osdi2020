CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy

GENERAL_FLAGS =
C_FLAGS = -Wall -Wextra -Wshadow -Wconversion -nostdlib -nostartfiles -mstrict-align
ASM_FLAGS =

KERNEL_C_SOURCE_FILES = string_util.c mailbox.c uart.c exc.c sys.c mmu.c buddy.c slab.c vfs.c tmpfs.c sd.c
KERNEL_C_OBJECT_FILES = $(KERNEL_C_SOURCE_FILES:.c=.o)
KERNEL_ASSEMBLY_SOURCE_FILES = start.S exc_vector.S mmu.S
KERNEL_ASSEMBLY_OBJECT_FILES = $(KERNEL_ASSEMBLY_SOURCE_FILES:.S=.asmo)

all: kernel8.img

kernel8.img: kernel8.elf
	${OBJCOPY} -O binary kernel8.elf kernel8.img

kernel8.elf: ${KERNEL_ASSEMBLY_OBJECT_FILES} ${KERNEL_C_OBJECT_FILES}
	${LD} -T kernel.ld $^ -o $@

%.o: %.c
	${CC} -c ${GENERAL_FLAGS} ${C_FLAGS} $^ -o $@

%.asmo: %.S
	${CC} -c ${GENERAL_FLAGS} ${ASM_FLAGS} $^ -o $@

debug: GENERAL_FLAGS += -DDEBUG -g3
debug: all

qemu: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio -drive if=sd,file=sfn_nctuos.fat32,format=raw

copy: all
	sudo mount /dev/sdb1 /mnt/ && sudo cp kernel8.img /mnt/ && sudo umount /mnt

.PHONY: qemu_dbg
qemu_dbg: debug
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -s -S -serial pty

.PHONY: clean
clean:
	rm -rf *.o *.elf *.img *.asmo
