#include "shell.h"
#include "uart.h"
#include "reboot.h"
#include "mbox.h"
#include "string.h"

// say hello
void shell_welcome_msg() {
	uart_puts("\n\n\n");
	uart_puts("                                        |\\__/,|   (`\\  \n");
	uart_puts("                                      _.|o o  |_   ) ) \n");
	uart_puts("|************************************(((***(((********|\n");
    uart_puts("|* Hello World! This is OSDI Lab 2 from Waylon Shih! *|\n");
	uart_puts("|*****************************************************|\n");
    uart_puts("\n\n# ");
}
	
// simple shell
void shell_start() {    
	char buf[40];
	char history[40];
	int len = 0;

	setstr(buf, 0, 40);
	setstr(history, 0, 40);
	
	shell_welcome_msg();

    while(1) {
		char c = uart_getc();
		// newline, enter command
		if(c == '\n') {
			uart_puts("\r\n");
		
			// read command
			cmdSwitch(buf);
			
			// reset buffer
			cpystr(history, buf, len);
			setstr(buf, 0, len);
			len = 0;

			uart_puts("# ");
		}
		// backspace
		else if(c == 127) {
			if(len > 0) {
				uart_puts("\b \b");
				buf[--len] = 0;
			}
		}
		// arrow keys
		else if(c == 27 && uart_getc() == '[') {
			char dir = uart_getc();
			// up
			if(dir == 'A');
			// down
			else if(dir == 'B');
			// right
			else if(dir == 'C');
			// left
			else if(dir == 'D');
		}
		else {
			uart_send(c);
			buf[len++] = c;
		}
    }
}

// handle commands
void cmdSwitch(char* buf) {

	if(!cmpstr(buf, "help"))
		cmd_help();
	else if(!cmpstr(buf, "hello"))
		uart_puts("Hello world!\n");
	else if(!cmpstr(buf, "timestamp"))
		cmd_time();
	else if(!cmpstr(buf, "reboot"))
		cmd_reboot();
	else if(!cmpstr(buf, "loadimg"))
		cmd_load();
	else if(!cmpstr(buf, "hwinfo"))
		cmd_hardwareInfo();
	else if(!cmpstr(buf, "")) ;
	else
		cmd_err(buf);
}


/* simple shell functions */
void cmd_help() {
	uart_puts("hello     : print Hello world!\n");
	uart_puts("help      : help\n");
	uart_puts("reboot    : reboot rpi3\n");
	uart_puts("timestamp : get current timestamp\n");
	uart_puts("\n");
	uart_puts("hwinfo    : print hardware info\n");
}

// DONE!!!!!!!!!!
void cmd_time() {
	unsigned long long count;
	unsigned long long freq;
	// get register value
	asm volatile("mrs %0, cntpct_el0" : "=r" (count));
	asm volatile("mrs %0, cntfrq_el0" : "=r" (freq));

	unsigned int quot = count / freq;
	unsigned int rem = count % freq;
	char left[20];
	char right[20];

	uart_puts("[");
	uart_puts(itoa(quot, left)); // integer part
	uart_puts(".");
	uart_puts(ftoa(rem, freq, right)); // fractional part
	uart_puts("]\n");
	
}

void cmd_reboot() {
	uart_puts("Reboot . . .\n");
	reset(100);
	do{asm volatile("nop");} while(1);

}

void cmd_load() {
	char tmp[100];
	// send the message to the GPU and receive answer
    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("My serial number is: ");
        uart_puts(itoa(mbox[6], tmp));
        uart_puts(itoa(mbox[5], tmp));
        uart_puts("\n");
    } else {
        uart_puts("Unable to query serial!\n");
    }	
}

void cmd_hardwareInfo() {
	int serial;
	int revision;
	int vcBase;
	int vcSize;
	uart_puts("Serial number: ");
	uart_puts("Board revision: ");
	uart_puts("VC memory base address: ");
	uart_puts("VC memory size: ");
}

void cmd_err(char* buf) {
	uart_puts("Err: command \'");
	uart_puts(buf);
	uart_puts("\' not found, try <help>.\n");
	//cmd_debug(buf);
}

void cmd_debug(char* buf) {
	char tmp[5];
	while(*buf) {
		uart_puts(itoa((int)*buf, tmp));
		uart_puts(" ");
		buf++;
	}
	uart_puts("\n");
}
	
