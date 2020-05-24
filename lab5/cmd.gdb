file kernel8.elf
target remote :1234
b *0x80038
b *0x80070
b *0x81100
b *0x81584