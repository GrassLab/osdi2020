EXEC = kernel8
CFLAGS = -std=gnu11
INCLUDE = -Iinclude -g3

KERNEL = kernel

LIBDIR = lib/
LIB_C := $(shell find $(LIBDIR) -name '*.c')
LIB_ASM := $(shell find $(LIBDIR) -name '*.s')

OBJS := $(LIB_C:%.c=%.o) \
        $(LIB_ASM:%.s=%.o) \
        $(KERNEL:=.o)

all: $(EXEC).img

start.o: start.s
	aarch64-linux-gnu-gcc -o $@ -c -O0 $<

%.o: %.s
	aarch64-linux-gnu-gcc -o $@ -c $<

%.o: %.c
	aarch64-linux-gnu-gcc -o $@ $(INCLUDE) $(CFLAGS) -c $<

$(EXEC).img: start.o $(OBJS)
	aarch64-linux-gnu-ld -o $(EXEC).elf -T linker.ld $^
	aarch64-linux-gnu-objcopy -O binary $(EXEC).elf $@

clean:
	$(RM) $(EXEC:=.elf) $(OBJS)
