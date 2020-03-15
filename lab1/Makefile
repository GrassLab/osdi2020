GENERAL_FLAGS =
OBJECT_FILES = start.o

all: ${OBJECT_FILES}
	aarch64-linux-gnu-ld -T linker.ld start.o -o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

%.o: %.s
	aarch64-linux-gnu-gcc -c ${GENERAL_FLAGS} $^ -o $@

debug: GENERAL_FLAGS += -DDEBUG -g3
debug: all

qemu:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm

qemu_dbg:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -s -S

.PHONY: clean
clean:
	rm -rf *.o *.elf *img
