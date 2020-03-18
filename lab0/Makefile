ARMGNU ?= aarch64-linux-gnu

COPS = -Wall

all: kernel8.img

clean:
	rm *.img

a.o: a.S
	$(ARMGNU)-gcc -c $<

kernel8.elf: a.o
	$(ARMGNU)-ld -T linker.ld -o $@ $<

kernel8.img: kernel8.elf
	$(ARMGNU)-objcopy -O binary $< $@
