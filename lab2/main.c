#include "system.h"
#include "util.h"
#include "uart.h"
#include "mbox.h"

#define	CMD_HELP	"help"
#define	CMD_HELLO	"hello"
#define	CMD_TIME	"timestamp"
#define	CMD_REBOOT	"reboot"
#define CMD_LOADIMG	"loadimg"
#define CMD_PICTURE "picture"

void get_timestamp();

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

void main()
{
	uart_init();
	// char *welcome = " _ _ _   _   __  ___   __  _ _  _  _ _ \n| | | | / \\ |  \\| __| / _|| U || || U |\n| V V || o || o ) _|  \\_ \\|   || ||   |\n \\_n_/ |_n_||__/|___| |__/|_n_||_||_n_|\n\n";
	char *welcome = "\n                        ___                  _ _ _        \n                       |__ \\                (_|_) |       \n   _ __ _   _ _ __ ___    ) |_ __ ___   ___  _ _| |_ ___  \n  | '__| | | | '_ ` _ \\  / /| '_ ` _ \\ / _ \\| | | __/ _ \\ \n  | |  | |_| | | | | | |/ /_| | | | | | (_) | | | || (_) |\n  |_|   \\__,_|_| |_| |_|____|_| |_| |_|\\___/| |_|\\__\\___/ \n                                           _/ |           \n                                          |__/            \n";
	uart_puts(welcome);

	get_board_revision();
	get_vc_core_base_addr();

	while(1) {
		uart_puts("#");
		char command[20];
		uart_read_line(command);
		uart_puts("\r");
		if(strcmp(command, CMD_HELP)) {
			char *help = "help:\t\t help";
			char *hello = "hello:\t\t print Hello World!";
			char *timestamp = "timestamp:\t get current timestamp";
			char *reboot = "reboot:\t\t reboot rpi3";
			char *loadimg = "loadimg:\t load image to rpi3";
			uart_puts(help);
			uart_puts("\r\n");
			uart_puts(hello);
			uart_puts("\r\n");
			uart_puts(timestamp);
			uart_puts("\r\n");
			uart_puts(reboot);
			uart_puts("\r\n");
			uart_puts(loadimg);
		} else if(strcmp(command, CMD_HELLO)) {
			uart_puts("Hello World!");
		} else if(strcmp(command, CMD_LOADIMG)) {
			uart_puts("[rpi3]\tStart Loading kernel image...\r\n");
			uart_puts("[rpi3]\tPlease input kernel load address (press <Enter> to use default addr: 0x80000):\r\n");
			char input_addr[20];
			uart_read_line(input_addr);
			if(strcmp(input_addr, "")) {
				input_addr[20] = "80000";
			}
			char res_str[100];
			
			load_image(input_addr);
		}  else if(strcmp(command, CMD_TIME)) {
			get_timestamp();
		}  else if(strcmp(command, CMD_REBOOT)) {
			reset();
		}  else if(strcmp(command, CMD_PICTURE)) {
			lfb_init();
			lfb_showpicture();
		} else {
			uart_puts("ERROR: ");
			uart_puts(command);
			uart_puts(" command not found! try <help>");
		}

		uart_puts("\r\n");
	}
}

void get_timestamp()
{
	register unsigned long f, c;
	asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
	asm volatile ("mrs %0, cntpct_el0" : "=r"(c));
	char res[30];
	ftoa(((float)c/(float)f), res, 10);
	uart_puts(res);
}
