#include "tools.h"
#include "uart.h"
#include "mbox.h"
#include "get_information.h"
#include "gpio.h"
#include "load_kernel.h"
#include "utils.h"

// for reboot
#define PM_PASSWORD (0x5a000000)
#define PM_RSTC ((int*)0x3F10001c)
#define PM_WDOG ((int*)0x3F100024)

#define  TMP_KERNEL_ADDR  0x00100000

#define JMP(new_address) __asm__("mov pc,%0" : /*output*/ : /*input*/ "r" (new_address) );

// linker variable
extern char __kernel_begin[];
extern char __bss_start[];
extern char __bss_end[];
extern char __bss_size[];
//extern volatile unsigned char __bss_size[];

void output_garbage(char *new_address, unsigned long long int new_size){
	
	char *iter = "";	

	char *kernel_size = "";
	uart_puts("\rThe new kernel size is ");
	itoa(new_size, kernel_size, 10);
	uart_puts(kernel_size);
	uart_puts("\n");
		
	uart_puts("\rStart loading kernel at ");
	uart_puts(new_address);
	uart_puts("\n");

	// handshake
	char *syn_message = "";
	uart_get_string(syn_message);
	if(!strcmp(syn_message, "SYN")){
		uart_puts("ACK");
	}
	
	for(int i=0; i<new_size; i++){
		new_address[i] = uart_getc();
		//itoa(i, iter, 10);
		//uart_puts(iter);
		//uart_puts("ACK");
	}
	uart_puts("already received all the data!!!\n");
	//JMP((unsigned long int *)524288);	
	//branch_to_address((unsigned long int *)__kernel_begin);
	
}

void copy_current_kernel_and_jump(char *new_address, unsigned long long int new_size) {
    char *kernel = __kernel_begin;
    char *end = __bss_end;
    char *copy = (char *)(TMP_KERNEL_ADDR);
	uart_puts("\rReady to copy the kernel to ");
    uart_puts("\rStart copying kernel to 0x00100000(temporary area)\n");
    while (kernel <= end) {
		//uart_puts("QQ\n");
        *copy = *kernel;
        kernel++; copy++;
    }
    uart_puts("\rCopying kernel is done!\n");
    void (*func_ptr)() = output_garbage;
    unsigned long int original_function_address = (unsigned long int)func_ptr;
    void (*call_function)(char *, unsigned long long int) = (void (*)(char *, unsigned long long int))(original_function_address - (unsigned long int)__kernel_begin + TMP_KERNEL_ADDR);
	call_function(new_address, new_size);
}

void main()
{
	// Initialization
    char buffer[1000];
    int i1 = 0, i2 = 0;
    int sizeof_current_line = 0;
    _Bool command_not_found = 1;
	unsigned long long int kernel_address;
	unsigned long long int kernel_size = 0;
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
			// char *buf = "", *buf1 = "";
			itoa(i1, buf1, 10);
			itoa(i2, buf2, 10);
			uart_puts("\r["); uart_puts(buf1); uart_puts(".");
			uart_puts(buf2); uart_puts("]"); uart_puts("\n");
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
			kernel_address =  read_kernel_address();
			kernel_size = read_kernel_size();
			char *tmp_address = "";
			itoa(kernel_address, tmp_address, 10);
			//copy_current_kernel_and_jump((char *)kernel_address, kernel_size);
			copy_current_kernel_and_jump(tmp_address, kernel_size);
			// copy_current_kernel_and_jump((char *)kernel_address);
			
			/*
			char * StartOfProgram1 = &__kernel_begin;
			// string_longlong_to_hex_char(string_buffer, (long long)&kernel_begin);
			uart_puts("\rThe kernel begins at: ");
			for(int i=0; i<10; i++){
				uart_send(StartOfProgram1[i]);
			}
			uart_puts(StartOfProgram1);
			uart_puts("\n");
			
			volatile unsigned char * const StartOfProgram2 = &__bss_start;
			uart_puts("The bss begins at: ");
			for(int i=0; i<10; i++){
				uart_send(StartOfProgram2[i]);
			}
			uart_puts(StartOfProgram2);
			uart_puts("\n");
			*/

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
