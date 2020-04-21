#include "shell.h"
#include "uart.h"
#include "reboot.h"
#include "mbox.h"
#include "string.h"
#include "lfb.h"
#include "msg.h"
#include "irq.h"

// say hello
void shell_welcome_msg() {
	uart_puts(welcome_msg);
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

	if (!cmpstr(buf, "help"))
		cmd_help();
	else if( !cmpstr(buf, "hello"))
		uart_puts("Hello world!\n");
	else if (!cmpstr(buf, "timestamp"))
		cmd_time();
	else if (!cmpstr(buf, "reboot"))
		cmd_reboot();
	else if (!cmpstr(buf, "loadimg"))
		cmd_load();
	else if (!cmpstr(buf, "hwinfo"))
		cmd_hardwareInfo();
	else if (!cmpstr(buf, "framebuf"))
		cmd_framebuf();
	else if (!cmpstr(buf, "getpel"))
		cmd_getEl();
	else if (!cmpstr(buf, "exc"))
		asm("svc #1");
	else if (!cmpstr(buf, "irq"))
		cmd_irq();

	else if(!cmpstr(buf, "")) ;
	else
		cmd_err(buf);
}


/* simple shell functions */
void cmd_help() {
	uart_puts(help_msg);
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
	
}

void cmd_hardwareInfo() {

    mbox[0] = 17 * 4;               // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
	mbox[2] = MBOX_TAG_GETREVISION; // get board revision command
    mbox[3] = 4;                    // buffer size
    mbox[4] = 0;                    // request code
    mbox[5] = 0;                    // clear output buffer
    
	mbox[6] = MBOX_TAG_GETSERIAL;
	mbox[7] = 8;
	mbox[8] = 0;
	mbox[9] = 0;
	mbox[10] = 0;

	mbox[11] = MBOX_TAG_GETVCMEM;
	mbox[12] = 8;
	mbox[13] = 0;
	mbox[14] = 0;
	mbox[15] = 0;
	
	mbox[16] = MBOX_TAG_LAST;
	mbox_call(MBOX_CH_PROP);

	uart_puts("Board revision: 0x");
	uart_hex(mbox[5]);
	uart_puts("\nSerial number: 0x");
	uart_hex(mbox[10]);
	uart_hex(mbox[9]);
	uart_puts("\nVC memory base address: 0x");
	uart_hex(mbox[15]);
	uart_puts("\nVC memory size: 0x");
	uart_hex(mbox[14]);
	uart_puts("\n");

//	for(int i=0; i<17; i++) {
//		uart_puts("mbox[");
//		uart_puts(itoa(i, tmp));
//		uart_puts("]: ");
//		uart_puts(itoa(mbox[i], tmp));
//		uart_puts("\n");
//	}
}

void cmd_framebuf() {
	lfb_init();
	uart_puts("Showing image...\n");
	lfb_showpicture();
}

void cmd_getEl() {
	unsigned long el;
	asm volatile ("mrs %0, CurrentEL" : "=r" (el));

	uart_puts("Current EL is: ");
	uart_hex((el>>2)&3);
	uart_puts("\n");

}

void cmd_irq() {
	timer_irq_print(1);

	// enable timer irq
	asm volatile ("mov x8, #0"); // system call number
	asm volatile ("svc #0"); // system call

	// run until press ENTER
	while(uart_getc() != '\n');

	timer_irq_print(0);
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
	
