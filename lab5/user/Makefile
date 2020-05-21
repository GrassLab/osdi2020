CFLAGS = -Wall -ffreestanding -nostdinc -nostdlib -nostartfiles -mgeneral-regs-only

all: clean user.img

str.o: str.c
	aarch64-linux-gnu-gcc $(CFLAGS) -g -c str.c -o str.o
user_lib.o: user_lib.c
	aarch64-linux-gnu-gcc $(CFLAGS) -g -c user_lib.c -o user_lib.o
userS.o: userS.S
	aarch64-linux-gnu-gcc $(CFLAGS) -g -c userS.S -o userS.o
user.o: user.c
	aarch64-linux-gnu-gcc $(CFLAGS) -fno-zero-initialized-in-bss -g -c user.c -o user.o
user.img: user.o userS.o user_lib.o str.o
	aarch64-linux-gnu-ld -T linker.ld -o user.elf str.o user.o userS.o user_lib.o
	aarch64-linux-gnu-objcopy -O binary user.elf user.img
	aarch64-linux-gnu-ld -r -b binary user.img -o user.rw

clean:
	rm user.img *.o >/dev/null 2>/dev/null || true