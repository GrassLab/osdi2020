file ./build/kernel8.elf
target remote :1234
b mm.c:237
watch *0xa9b08
continue
