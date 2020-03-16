EXEC = kernel8
ASM_SRC = memzero
C_SRC = kernel

SRC := $(ASM_SRC) \
       $(C_SRC)

all: $(EXEC).img

start.o: start.s
	aarch64-linux-gnu-gcc -o $@ -c -O0 $< 

%.o: %.s
	aarch64-linux-gnu-gcc -o $@ -c $<

%.o: %.c
	aarch64-linux-gnu-gcc -o $@ -c $<

$(EXEC).img: start.o $(SRC:=.o)
	aarch64-linux-gnu-ld -o $(EXEC).elf -T linker.ld $^
	aarch64-linux-gnu-objcopy -O binary $(EXEC).elf $@ 

clean:
	$(RM) $(EXEC:=.elf) *.o
