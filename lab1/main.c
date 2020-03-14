#include "system.h"
#include "util.h"
#include "uart.h"

#define	CMD_HELP	"help"
#define	CMD_HELLO	"hello"
#define	CMD_TIME	"timestamp"
#define	CMD_REBOOT	"reboot"

long get_timestamp();
void uart_read_line(char *line);

void
main()
{
	uart_init();

	while(1) {
		uart_puts("#");
		char command[20];
		uart_read_line(command);
		if(strcmp(command, CMD_HELP)) {
			char help[] = "help:\t\t help";
			char hello[] = "hello:\t\t print Hello World!";
			char timestamp[] = "timestamp:\t get current timestamp";
			char reboot[] = "reboot:\t\t reboot rpi3";
			uart_puts(help);
			uart_send('\n');
			uart_puts(hello);
			uart_send('\n');
			uart_puts(timestamp);
			uart_send('\n');
			uart_puts(reboot);
		} else if(strcmp(command, CMD_HELLO)) {
			uart_puts("Hello World!");
		} else if(strcmp(command, CMD_TIME)) {
			get_timestamp();
		} else if(strcmp(command, CMD_REBOOT)) {
			reset();
		} else {
			uart_puts("ERROR: ");
			uart_puts(command);
			uart_puts(" command not found! try <help>");
		}

		uart_send('\n');
	}
}

long
get_timestamp()
{
	register unsigned long f, c;
	// get the current counter frequency
	asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
	asm volatile ("mrs %0, cntpct_el0" : "=r"(c));
	char res[30];
	ftoa(((float)c/(float)f), res, 10);
	uart_puts(res);
	return c/f;
}

void
uart_read_line(char *line)
{
	char c;
	int index = 0;

	while(c != '\n') {
		c = uart_getc();
		uart_send(c);
		if(c == '\n') {
			line[index] = '\0';
		} else {
			line[index] = c;
		}
		index++;
	}
}
