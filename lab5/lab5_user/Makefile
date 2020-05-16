COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only -std=gnu99 -ggdb
all: user_program
printf.o : printf.c
	aarch64-linux-gnu-gcc $(COPS) -fno-zero-initialized-in-bss -c printf.c -o printf.o

user_lib_c.o : user_lib.c
	aarch64-linux-gnu-gcc $(COPS) -fno-zero-initialized-in-bss -c user_lib.c -o user_lib_c.o

user_lib_s.o : user_lib_s.S
	aarch64-linux-gnu-gcc $(COPS) -fno-zero-initialized-in-bss -c user_lib_s.S -o user_lib_s.o

user_program.o : user_program.c
	aarch64-linux-gnu-gcc $(COPS) -fno-zero-initialized-in-bss -c user_program.c -o user_program.o

user_program.elf : printf.o user_lib_s.o user_lib_c.o user_program.o
	aarch64-linux-gnu-ld -T linker.ld -o user_program.elf printf.o user_lib_s.o user_lib_c.o user_program.o

user_program : user_program.elf
	aarch64-linux-gnu-objcopy user_program.elf -O binary user_program

clean :
	rm -rf *.o 