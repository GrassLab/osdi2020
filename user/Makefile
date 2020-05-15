ARMGNU = aarch64-linux-gnu-
CC = $(ARMGNU)gcc
LDFLAGS =
HEADER := $(wildcard lib/*.h) $(wildcard include/*.h)
SRC := $(wildcard lib/*.c)
ASM := $(wildcard lib/asm/*.S)
OBJECTS := $(patsubst %.S,%.o,$(ASM)) $(patsubst %.c,%.o,$(SRC))
CFLAGS = -nostdlib -Ilib -Iinclude -Wextra
ASFLAGS =

all: bin_shell.o

program/shell: $(OBJECTS) include/stdlib.h program/shell.c

bin_shell.o: program/shell
	$(ARMGNU)objcopy -O binary program/shell bin_shell
	$(ARMGNU)ld -r -b binary -o bin_shell.o bin_shell
