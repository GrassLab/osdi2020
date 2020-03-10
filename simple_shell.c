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

// TODO: empty input, parameter parse
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

void show_timestamp()
{
    uart_puts("timestamp");
}

void reboot_rpi3()
{
    uart_puts("reboot");
}

void print_prompt()
{
	uart_puts(promptStr);
}