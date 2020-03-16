EXEC = kernel8
SRC = memzero

all: $(EXEC).img

start.o: start.s
	aarch64-linux-gnu-gcc -o $@ -c -O0 $< 

%.o: %.s
	aarch64-linux-gnu-gcc -o $@ -c $<

$(EXEC).img: start.o $(SRC:=.o)
	aarch64-linux-gnu-ld -o $(EXEC).elf -T linker.ld $^
	aarch64-linux-gnu-objcopy -O binary $(EXEC).elf $@ 

clean:
	$(RM) $(EXEC:=.elf) *.o
