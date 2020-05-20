SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFALG = -fPIC -Wall -ffreestanding -nostdinc -nostdlib -nostartfiles -Iinclude -g -O0 -ggdb3 -MMD
ASMFALG = -fPIC -Iinclude -g -O0 -ggdb3 -MMD
SRC_DIR = src
BUILD_DIR = build
USER_DIR = user_code

all: clean kernel8.img


$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	aarch64-linux-gnu-gcc $(CFALG) -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	aarch64-linux-gnu-gcc $(ASMFALG) -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)


$(USER_DIR)/user_c.o: $(USER_DIR)/user.c
	aarch64-linux-gnu-gcc -fno-zero-initialized-in-bss -nostdlib -g -fPIC -c $< -o $@

$(USER_DIR)/user_lib.o: $(USER_DIR)/user_lib.S
	aarch64-linux-gnu-gcc -fPIC -g -c $< -o $@

user_program: $(USER_DIR)/user_c.o $(USER_DIR)/user_lib.o 
	aarch64-linux-gnu-ld -T $(USER_DIR)/link.ld -o $(USER_DIR)/test.elf $(USER_DIR)/user_c.o $(USER_DIR)/user_lib.o 
	aarch64-linux-gnu-objcopy -O binary $(USER_DIR)/test.elf $(USER_DIR)/test.img 
	aarch64-linux-gnu-ld -r -b binary -o user_program $(USER_DIR)/test.img

build_dir:
	mkdir -p build

kernel8.img: build_dir $(OBJ_FILES) user_program
	aarch64-linux-gnu-ld -T $(SRC_DIR)/l.ld -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES) user_program
	aarch64-linux-gnu-objcopy -O binary $(BUILD_DIR)/kernel8.elf kernel8.img



.PHONY: clean run runasm runpty gdb 

clean:
	rm -rf $(BUILD_DIR) *.img user_code/*.o user_code/*.elf user_code/*.img

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial stdio

runpty:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial pty

runasm:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm -serial stdio

rundis:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio

rungdb:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial stdio -S -gdb tcp::1234

gdb:
	aarch64-linux-gnu-gdb build/kernel8.elf

dump:
	aarch64-linux-gnu-objdump -S build/kernel8.elf