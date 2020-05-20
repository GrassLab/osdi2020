#
# Copyright (C) 2018 bzt (bztsrc@github)
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use, copy,
# modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#
#

C_SRCS = $(wildcard *.c)
C_OBJS = $(C_SRCS:.c=.o)
S_SRCS = $(wildcard *.S)
S_OBJS = $(S_SRCS:.S=.s.o)
KER_OBJ = ../irq.s.o 
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -I../ -fno-zero-initialized-in-bss -ggdb3 -Og



all: clean user8.img

%.s.o: %.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

%.o: %.c 
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

user8.img: $(S_OBJS) $(C_OBJS) 
	aarch64-linux-gnu-ld -nostdlib -nostartfiles $(S_OBJS) $(C_OBJS) $(KER_OBJ) -T link.ld -o user8.elf
	aarch64-linux-gnu-objdump user8.elf -d > disasm.out
	aarch64-linux-gnu-ld -r -b binary user8.elf -o user8.rd

clean:
	rm user8.elf *.o >/dev/null 2>/dev/null || true
	rm user8.elf *.s.o >/dev/null 2>/dev/null || true