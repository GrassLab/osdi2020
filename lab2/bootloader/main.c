#include "uart.h"

void main()
{
	int size = 0;
	char *kernel = (char *)0x60000;
	uart_init();
again:

	// read the kernel's size
	size = uart_getc();
	size |= uart_getc() << 8;
	size |= uart_getc() << 16;
	size |= uart_getc() << 24;
	// send negative or positive acknowledge
	if (size < 64 || size > 1024 * 1024) {
		// size error
		uart_send('S');
		uart_send('E');
		// uart_send(0x04);
		goto again;
	}
	// uart_send(0x06);
	uart_send('O');
	uart_send('K');

	// read the kernel
	while (size--)
		*kernel++ = uart_getc();

	// restore arguments and jump to the new kernel.
	asm volatile("mov x0, x10;"
		     "mov x1, x11;"
		     "mov x2, x12;"
		     "mov x3, x13;"
		     // we must force an absolute address to branch to
		     "mov x30, 0x60000; ret");
}
