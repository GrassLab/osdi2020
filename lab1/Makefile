EXEC = kernel8

all: $(EXEC).img

start.o: start.s
	aarch64-linux-gnu-gcc -o $@ -c -O0 $< 

$(EXEC).img: start.o
	aarch64-linux-gnu-ld -o $(EXEC).elf -T linker.ld $<
	aarch64-linux-gnu-objcopy -O binary $(EXEC).elf $@ 

clean:
	$(RM) $(EXEC:=.elf) *.o
