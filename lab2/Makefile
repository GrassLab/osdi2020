CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy

GENERAL_FLAGS =
C_FLAGS = -Wall -Wextra -Wshadow -Wconversion -nostdlib -nostartfiles

BOOTLOADER_C_SOURCE_FILES = bootloader.c uart.c string_util.c mailbox.c loadimg.c sumhash.c
BOOTLOADER_C_OBJECT_FILES = $(BOOTLOADER_C_SOURCE_FILES:.c=.o)
BOOTLOADER_ASSEMBLY_SOURCE_FILES = start.s
BOOTLOADER_ASSEMBLY_OBJECT_FILES = $(BOOTLOADER_ASSEMBLY_SOURCE_FILES:.s=.o)
KERNEL_C_SOURCE_FILES = main.c shell.c string_util.c mailbox.c uart.c
KERNEL_C_OBJECT_FILES = $(KERNEL_C_SOURCE_FILES:.c=.o)
KERNEL_ASSEMBLY_SOURCE_FILES = start.s get_time.s
KERNEL_ASSEMBLY_OBJECT_FILES = $(KERNEL_ASSEMBLY_SOURCE_FILES:.s=.o)

all: kernel8.img os.img

os.img: os.elf
	${OBJCOPY} -O binary os.elf os.img

os.elf: ${KERNEL_ASSEMBLY_OBJECT_FILES} ${KERNEL_C_OBJECT_FILES}
	${LD} -T os.ld ${KERNEL_ASSEMBLY_OBJECT_FILES} ${KERNEL_C_OBJECT_FILES} -o os.elf

kernel8.img: kernel8.elf
	${OBJCOPY} -O binary kernel8.elf kernel8.img

kernel8.elf: ${BOOTLOADER_ASSEMBLY_OBJECT_FILES} ${BOOTLOADER_C_OBJECT_FILES}
	${LD} -T bootloader.ld ${BOOTLOADER_ASSEMBLY_OBJECT_FILES} ${BOOTLOADER_C_OBJECT_FILES} -o kernel8.elf

%.o: %.c
	${CC} -c ${GENERAL_FLAGS} ${C_FLAGS} $^ -o $@

%.o: %.s
	${CC} -c ${GENERAL_FLAGS} $^ -o $@

debug: GENERAL_FLAGS += -DDEBUG -g3
debug: all

qemu_bl: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial stdio

qemu_bl_pty: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial pty

qemu_bl_dbg: debug
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -s -S -serial pty

qemu_os: all
	qemu-system-aarch64 -M raspi3 -kernel os.img -display none -serial stdio

qemu_os_dbg: debug
	qemu-system-aarch64 -M raspi3 -kernel os.img -display none -s -S -serial pty

.PHONY: clean
clean:
	rm -rf *.o *.elf *img

