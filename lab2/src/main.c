#include "tools.h"
#include "uart.h"
#include "mbox.h"
#include "get_information.h"
#include "gpio.h"
#include "load_kernel.h"
#include "utils.h"
#include "lfb.h"

// linker variable
extern unsigned char __kernel_begin;
extern unsigned char __bss_start;
extern unsigned char __bss_end;
extern unsigned char __bss_size;
//extern volatile unsigned char __bss_size[];

// framebuffer extern settings
extern unsigned int width, height, pitch, isrgb;
extern unsigned char *lfb;


// functions
void output_garbage(unsigned long long int new_kernel_address, unsigned long long int new_kernel_size);
void copyOldKernel2TempSpace(unsigned long long int new_kernel_address, unsigned long long int new_kernel_size);


void output_garbage(unsigned long long int new_kernel_address, unsigned long long int new_kernel_size){
	// print and check the size of the new kernel
	char *new_size = "";
	uart_puts("\rThe new kernel size is ");
	itoa(new_kernel_size, new_size, 10);
	uart_puts(new_size);
	uart_puts("\n");
	// print and check the new kernel start address
	char *new_address = "";
	uart_puts("\rStart loading kernel at ");
	itoa(new_kernel_address, new_address, 10);
	uart_puts(new_address);
	uart_puts("\n");
	// handshake
	char *syn_message = "";
	uart_get_string(syn_message);
	if(!strcmp(syn_message, "SYN")){
		uart_puts("ACK");
	}
	// start receiving new kernel
	volatile char *kernel_addr = (void *)new_kernel_address;
	int checksum = 0;
	unsigned char c;
	for(int i=0; i<new_kernel_size; i++){
		c = uart_getc();
		checksum = checksum + c;
		kernel_addr[i] = c;
	}
	// print and check the checksum
	char *checksum_str = "";
	itoa(checksum, checksum_str, 10);
	uart_puts("rpi3 check sum: ");
	uart_puts(checksum_str);
	uart_puts("\nalready received all the data!!!\n");
	// jump to specified address
	void (*func_ptr)() = (void (*)())((char *)new_kernel_address);
	asm volatile("mov sp, %0" :: "r"((void *)new_kernel_address));
	func_ptr();


	//JMP((unsigned long int *)524288);
	//branch_to_address((unsigned long int *)new_address);
	//asm volatile("br %0" : "=r"((unsigned long int *)new_address));
/*
	void (*start_os)(void) = 0x00080000;
  	start_os();
*/
}

void copyOldKernel2TempSpace(unsigned long long int new_kernel_address, unsigned long long int new_kernel_size){

	// set the temporary address for old kernel 
	unsigned char *tempAddrForOldKernel = (unsigned char *)(0x00100000);
	// COPY OLD KERNEL
	unsigned char *start = &__kernel_begin;
    unsigned char *end = &__bss_end;
	uart_puts("\rReady to copy the kernel\n");
    uart_puts("\rStart copying kernel to 0x00100000(temporary area)\n");
// *****************************************************************************	
	memcpy(tempAddrForOldKernel, &__kernel_begin, &__bss_end - &__kernel_begin);
// *****************************************************************************	
    uart_puts("\rCopying is done!\n");

	// jump to temporary address
	//void (*func_ptr)() = (void (*)())(tempAddrForOldKernel);
	//func_ptr();

	void (*func_ptr)() = output_garbage;
	unsigned long int original_function_address = (unsigned long int)func_ptr;
	void (*call_function)(unsigned long long int, unsigned long long int) = (void (*)(unsigned long long int, unsigned long long int))(original_function_address - (unsigned long long int)(&__kernel_begin) + 0x00100000);
	call_function(new_kernel_address, new_kernel_size);
}

void main()
{
	// Initialization
    char buffer[1000];
    int i1 = 0, i2 = 0;
    int sizeof_current_line = 0;
    _Bool command_not_found = 1;
	unsigned long long int new_kernel_address = 0;
	unsigned long long int new_kernel_size = 0;
	char buf1[1000], buf2[1000];
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
			uart_puts("\rHello World 1 !\n");
			command_not_found = 0;
		}else if(!strcmp(buffer, "\nhello")) {
			uart_puts("\rHello World 4 !\n");
			command_not_found = 0;
		}else if(!strcmp(buffer, "help")) {
			uart_puts("\rhello: print Hello World!\n");
			uart_puts("help: help\n");
			uart_puts("reboot: reboot rpi3\n");
			uart_puts("timestamp: get current timestamp\n");
			uart_puts("framebuffer: show the framebuffer\n");
			uart_puts("loadimg: load kernel at specified address\n");
			command_not_found = 0;
		}else if(!strcmp(buffer, "\nhelp")) {
			uart_puts("\rhello: print Hello World!\n");
			uart_puts("help: help\n");
			uart_puts("reboot: reboot rpi3\n");
			uart_puts("timestamp: get current timestamp\n");
			uart_puts("framebuffer: show the framebuffer\n");
			uart_puts("loadimg: load kernel at specified address\n");
			command_not_found = 0;
		}else if(!strcmp(buffer, "timestamp")) {
			timestamp(&i1, &i2);
			// turn the number into string
			// char *buf = "", *buf1 = "";
			itoa(i1, buf1, 10);
			itoa(i2, buf2, 10);
			uart_puts("\r["); uart_puts(buf1); uart_puts(".");
			uart_puts(buf2); uart_puts("]"); uart_puts("\n");
			command_not_found = 0;
		}else if(!strcmp(buffer, "\ntimestamp")) {
			timestamp(&i1, &i2);
			// turn the number into string
			// char *buf = "", *buf1 = "";
			itoa(i1, buf1, 10);
			itoa(i2, buf2, 10);
			uart_puts("\r["); uart_puts(buf1); uart_puts(".");
			uart_puts(buf2); uart_puts("]"); uart_puts("\n");
			command_not_found = 0;
		}else if(!strcmp(buffer, "reboot")) {
			/* reboot rpi3 */
			*PM_RSTC = ((PM_PASSWORD) | 0x20); // full reset
  			*PM_WDOG = ((PM_PASSWORD) | 10); // number of watchdog tick
			
			command_not_found = 0;
			continue;
		}else if(!strcmp(buffer, "\nreboot")) {
			/* reboot rpi3 */
			*PM_RSTC = ((PM_PASSWORD) | 0x20); // full reset
  			*PM_WDOG = ((PM_PASSWORD) | 10); // number of watchdog tick
			
			command_not_found = 0;
			continue;
		}else if(!strcmp(buffer, "framebuffer")) {
			uart_puts("\rShow framebuffer\n");
			lfb_init();
			lfb_showpicture();
			command_not_found = 0;
		}else if(!strcmp(buffer, "\nframebuffer")) {
			uart_puts("\rShow framebuffer\n");
			lfb_init();
			lfb_showpicture();
			command_not_found = 0;
		}else if(!strcmp(buffer, "loadimg")) {
			// input the address where the new kernel loaded at(in hex format, e.g. 0x12345678)
			new_kernel_address =  read_new_kernel_address();
			// input the size of the new kernel
			new_kernel_size = read_new_kernel_size();
			// copy the old kernel to temporary space	
			copyOldKernel2TempSpace(new_kernel_address, new_kernel_size);
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
