#include "uart.h"

void load_image(char *input_address) {
	uart_puts("[rpi3]\tSet load address 0x");
	uart_puts(input_address);
	uart_puts("\r\n");
	uart_puts("[rpi3]\tPlease send kernel image from UART now...\r\n");

	
	
	char *load_address;
	int address_int = hex2int(input_address);

	void (* new_kernel)(void) = address_int;
	// new_kernel = (void (*)(void)) 0x80000;
	

	load_address = address_int;
	char *load_address_s;
	int kernel_size = uart_read_int();
	char *kernel_size_s;
	uart_puts("Kernel size: ");
	uart_puts(itoa(kernel_size, kernel_size_s, 10));
	uart_puts("\t");
	uart_puts("Load address: 0x");
	uart_puts(itoa(address_int, kernel_size_s, 16));
	uart_puts("\r\n");

	for(int i=0; i<kernel_size; i++) {
		unsigned char c = uart_getc();
		*load_address = c;
		uart_send(*load_address);
		load_address++;
	}
	unsigned char c = uart_getc();
	*load_address = c;
	uart_send(*load_address);
	uart_puts("done");
	// branchAddr(0x40000);
	
	uart_puts("[rpi3]\tDONE!\r\n");
	// TODO: jump to new kernel
	// waiting
	char read_buf[1024];
	uart_read_line(read_buf);
	// asm volatile("mov sp, %0" :: "r"(address_int));
	// void (* new_kernel)(void) = address_int;
	// new_kernel = (void (*)(void)) 0x80000;
	// for(int i=0; i<10000000; i++) {}
	// register unsigned int r;
	// r=5000; while(r--) { asm volatile("nop"); }
	// uart_puts("[rpi3]\tDONE!\r\n");
	new_kernel();
}