#include "simple_shell.h"

#include "mini_uart.h"


char* promptStr="simple shell> ";

char* commandStr[NUM_COMMAND]=
{
	"help", "hello", "timestamp", "reboot"//, "exit"
};

char* commandDesc[NUM_COMMAND]={
	"Show commands available.", "Show \"Hello World!\"", "Get current timestamp.", "Reboot device."
};

void (*commandArray[NUM_COMMAND])()=
{
	show_help, show_hello, show_timestamp, reboot_rpi3
};

// TODO: formated string
void run_shell()
{
	char buf[MAX_COMMAND_LENGTH];
	int n, cmd_type;

	print_prompt();
	while ((n=read_command(buf, MAX_COMMAND_LENGTH)) >= 0){
		buf[n]='\0';
		cmd_type = parse_command(buf);
		if (cmd_type==-1){
			uart_puts("command '");
			uart_puts(buf);
			uart_puts("' not found, try <help>\n");
		} else {
			commandArray[cmd_type]();
		}
    	uart_send('\n');
		print_prompt();
	}
}

// TODO: boundary test
int read_command(char* buffer, unsigned int max_len)
{
	int count = 0;
	char char_recv;
	while (count < max_len){
		char_recv = uart_getc();
		uart_send(char_recv);
		if (char_recv == '\n')
			return count;
		else if (char_recv == '\177'){// backspace
			if (count >= 1){
				uart_puts("\b \b");
				count --;
			}
		} else {
			buffer[count] = char_recv;
			count ++;
		}
	}
	return -1; // out of buff	
}

int _strcmp(const char* s1, const char* s2){
	for (; *s1 == *s2 ; s1++, s2++){
		if (*s1 == '\0')
			return 0;
	}
	return *s1 - *s2;
}

// TODO:  parameter parse
int parse_command(char* buf)
{
	int i;
	for (i=NUM_COMMAND-1; i>=0; i--){
		if (_strcmp(buf, commandStr[i])==0)
			break;
	}
	return i;
}

// TODO: formate string
void show_help()
{
	for(int i=0; i<NUM_COMMAND; i++){
		uart_puts(commandStr[i]);
		uart_puts(": ");
		uart_puts(commandDesc[i]);
		uart_puts("\n");
	}
}

void show_hello()
{
    uart_puts("Hello world!\n");
}

// TODO: need implement malloc for dynamic length array
// TODO: uding while version to eliminate leading zeros
void _unitoa(unsigned num, char* buf, unsigned num_dig){
	unsigned int ASCII_BIAS = 48;
	buf[num_dig] = '\0';
	for(int i=num_dig-1; i>=0; i--){
		buf[i] = (char)(num%10 + ASCII_BIAS);
		num = num / 10;
	}
}

// TODO: hard-coding num_digit
void show_timestamp()
{
	unsigned int time, time_count, time_freq;
	char buf[10];
	asm volatile("mrs %0, cntpct_el0": "=r"(time_count)::); // read counts of core timer
	asm volatile("mrs %0, cntfrq_el0": "=r"(time_freq)::); // read frequency of core timer
	time = time_count / (time_freq / 100000U);
	
	_unitoa((time/100000U), buf, 3);
	uart_puts(buf); // natural part
	uart_send('.');
	_unitoa(time%100000U, buf, 5);
	uart_puts(buf); // decimal part
	uart_send('\n');
}

void reboot_rpi3()
{
    uart_puts("reboot");
}

void print_prompt()
{
	uart_puts(promptStr);
}