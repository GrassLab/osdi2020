EXEC = kernel8
CFLAGS = -std=gnu11 -nostdlib
INCLUDE = -Iinclude -g3

KERNEL = kernel.c

LIBDIR = lib/
LIB_C := $(shell find $(LIBDIR) -name '*.c')
LIB_ASM := $(shell find $(LIBDIR) -name '*.s')

MMUDIR = lib/mmu/
MMU_ASM := $(shell find $(MMUDIR) -name '*.S')

OBJS := start.o \
        $(LIB_C:%.c=%_c.o) \
        $(LIB_ASM:%.s=%_s.o) \
		$(MMU_ASM:%.S=%_s.o) \
        $(KERNEL:%.c=%_c.o)

all: $(EXEC).img

start.o: start.s
	aarch64-linux-gnu-gcc -o $@ -c -O0 $<

%_s.o: %.s
	aarch64-linux-gnu-gcc -o $@ -c $<

%_c.o: %.c
	aarch64-linux-gnu-gcc -o $@ $(INCLUDE) $(CFLAGS) -c $<

$(EXEC).img: $(OBJS) linker.ld
	aarch64-linux-gnu-ld -o $(EXEC).elf -T linker.ld $(OBJS)
	aarch64-linux-gnu-objcopy -O binary $(EXEC).elf $@

clean:
	$(RM) $(EXEC:=.elf) $(OBJS)
