EXEC = kernel8
CFLAGS = -std=gnu11 -nostdlib
INCLUDE = -Iinclude -g3

KERNEL = kernel.c

LIBDIR = lib/
LIB_C := $(shell find $(LIBDIR) -name '*.c')
LIB_ASM := $(shell find $(LIBDIR) -name '*.s')

EXCEPTIONDIR = lib/exception/
EXCEPTION_ASM := $(shell find $(EXCEPTIONDIR) -name '*.s')

SCHEDULEDIR = lib/schedule/
SCHEDULE_C := $(shell find $(SCHEDULEDIR) -name '*.c')
SCHEDULE_ASM := $(shell find $(SCHEDULEDIR) -name '*.s')

OBJS := $(LIB_C:%.c=%_c.o) \
        $(LIB_ASM:%.s=%_s.o) \
        $(EXCEPTION_ASM:%.s=%_s.o) \
		$(SCHEDULE_C:%.c=%_c.o) \
		$(SCHEDULE_ASM:%.s=%_s.o) \
        $(KERNEL:%.c=%_c.o)

all: $(EXEC).img

start.o: start.s
	aarch64-linux-gnu-gcc -o $@ -c -O0 $<

%_s.o: %.s
	aarch64-linux-gnu-gcc -o $@ -c $<

%_c.o: %.c
	aarch64-linux-gnu-gcc -o $@ $(INCLUDE) $(CFLAGS) -c $<

$(EXEC).img: start.o $(OBJS)
	aarch64-linux-gnu-ld -o $(EXEC).elf -T linker.ld $^
	aarch64-linux-gnu-objcopy -O binary $(EXEC).elf $@

clean:
	$(RM) $(EXEC:=.elf) $(OBJS)
