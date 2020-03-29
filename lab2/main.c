#include "tools.h"
#include "uart.h"
#include "mbox.h"
#include "get_information.h"
#include "gpio.h"
#include "load_kernel.h"

#define PM_PASSWORD (0x5a000000)
#define PM_RSTC ((int*)0x3F10001c)
#define PM_WDOG ((int*)0x3F100024)

void main()
{
	// Initialization
    char buffer[1000];
    int i1, i2;
    int sizeof_current_line = 0;
    _Bool command_not_found = 1;
	unsigned long long int kernel_address;
	
	// UART0 initialization
    uart_init();
	uart_puts("\r\n");
	
	// Print the peripheral information
	get_serial_number();
	get_board_revision();
	get_VC_memory();
    

	// Welcome message
	uart_puts("\r\nWelcome to raspberry pi 3!\n# ");

	while(1){
		/* for pyserial interface */
		sizeof_current_line = uart_get_string(buffer);
		/* for qemu */
		//sizeof_current_line = uart_get_string_with_echo(buffer);
		command_not_found = 1;
		if(!strcmp(buffer, "hello")) {
			uart_puts("\rHello World!\n");
			command_not_found = 0;
		}
		else if(!strcmp(buffer, "help")) {
			uart_puts("\rhello: print Hello World!\n");
			uart_puts("help: help\n");
			uart_puts("reboot: reboot rpi3\n");
			uart_puts("timestamp: get current timestamp\n");
			uart_puts("framebuffer: show the framebuffer\n");
			uart_puts("loadimg: load kernel at specified address\n");
			command_not_found = 0;
		}
		else if(!strcmp(buffer, "timestamp")) {
			timestamp(&i1, &i2);
			// turn the number into string
			char buf[100], buf1[100];
			itoa(i1, buf, 10); itoa(i2, buf1, 10);
			uart_puts("\r["); uart_puts(buf); uart_puts(".");
			uart_puts(buf1); uart_puts("]"); uart_puts("\n");
			command_not_found = 0;
		}
		else if(!strcmp(buffer, "reboot")) {
			/* reboot rpi3 */
			*PM_RSTC = ((PM_PASSWORD) | 0x20); // full reset
  			*PM_WDOG = ((PM_PASSWORD) | 10); // number of watchdog tick
			
			command_not_found = 0;
			continue;
		}
		else if(!strcmp(buffer, "framebuffer")) {
			uart_puts("\rShow framebuffer\n");
			command_not_found = 0;
		}
		else if(!strcmp(buffer, "loadimg")) {
			// uart_puts("\rLoad kernel image\n");
			kernel_address =  read_kernel_address();
			//bss_start = ;
			//bss_end = ;
			//bss_size = ;
			
			command_not_found = 0;
		}
		if(command_not_found == 1 && sizeof_current_line>0) {
			uart_puts("\rError: command \"");
			uart_puts(buffer);
			uart_puts("\" not found. Try <help>\n");
		}
		uart_puts("\r# ");
		sizeof_current_line = 0;
    }
}
