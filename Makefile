ARMGCC = aarch64-linux-gnu
OBJ = main.c uart.c
CFLAGS = -Wall -ffreestanding -nostdlib -nostartfiles - mgeneral-regs-only
.PHONY: all clean

object.o: ${OBJ}
    ${ARMGCC}-gcc ${CFLAGS} -c ${OBJ}  -o object.o 
