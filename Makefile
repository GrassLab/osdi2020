CFLAGS = -ffreestanding -nostartfiles -nostdlib -g
INCS = -I .

ASM_FILES = $(shell find kernel -name "*.S")
C_FILES = $(shell find kernel -name "*.c")
OBJS = $(ASM_FILES:%.S=%_s.o) $(C_FILES:%.c=%_c.o)

USER_ASM_FILES = $(shell find user -name "*.S")
USER_C_FILES = $(shell find user -name "*.c")
USER_OBJS = $(USER_ASM_FILES:%.S=%_s.o) $(USER_C_FILES:%.c=%_c.o)

.PHONY: all clean

all: user_shell kernel8.img

kernel/%_c.o: kernel/%.c
	aarch64-linux-gnu-gcc $(INCS) $(CFLAGS) -c -o $@ $<

kernel/%_s.o: kernel/%.S
	aarch64-linux-gnu-gcc $(INCS) -c -o $@ $<

user/%_c.o: user/%.c
	aarch64-linux-gnu-gcc $(INCS) $(CFLAGS) -fno-zero-initialized-in-bss -c -o $@ $<

user/%_s.o: user/%.S
	aarch64-linux-gnu-gcc $(INCS) -c -o $@ $<

user_shell: $(USER_OBJS)
	aarch64-linux-gnu-ld -T user/linker.ld -o user_shell.elf $^
	aarch64-linux-gnu-objcopy -O binary user_shell.elf user_shell.img
	aarch64-linux-gnu-ld -r -b binary user_shell.img -o $@

kernel8.img: $(OBJS)
	aarch64-linux-gnu-ld -T kernel/linker.ld -o kernel8.elf $^ user_shell
	aarch64-linux-gnu-objcopy -O binary kernel8.elf $@

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial pty -drive if=sd,file=sfn_nctuos.img,format=raw

debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s -serial null -serial stdio -drive if=sd,file=sfn_nctuos.img,format=raw

clean:
	@rm -rf $(OBJS) $(USER_OBJS) kernel8.* user_shell*
