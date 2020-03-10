#include "simple_shell.h"

#include "mini_uart.h"

char* promptStr="simple shell> ";
char* commandStr[CommandNumber]=
{
	"help", "hello", "reboot", "timestamp"//, "exit"
};

// TODO: formated string
void run_shell()
{
	char buf[MaxCommandLine];
	int n, cmd_type;

	print_prompt();
	while ((n=read_command(buf, MaxCommandLine)) >= 0){
		buf[n]='\0';
		cmd_type = parse_command(buf);
		if (cmd_type==-1){
			uart_puts("command '");
			uart_puts(buf);
			uart_puts("' not found, try <help>\n");
		} else {
			uart_puts(buf);
			uart_send('\n');
		}
		print_prompt();
	}
}

// TODO: boundary test
int read_command(char* buffer, unsigned int max_len)
{
	int count = 0;
	char char_recv;
	while (count < max_len)
	{
		char_recv = uart_getc();
		uart_send(char_recv);
		if (char_recv == '\n')
			return count;
		buffer[count] = char_recv;
		count ++;
	}
	return -1; // out of buff	
}

int _strcmp(const char* s1, const char* s2){
	for (; *s1 != '\0' && *s2 != '\0'; s1++, s2++){
		if ((*s1 - *s2) != 0)
			return (*s1 - *s2);
	}
	return 0;
}

// TODO: empty input, parameter parse
int parse_command(char* buf)
{
	int i;
	for (i=CommandNumber-1; i>=0; i--){
		if (_strcmp(buf, commandStr[i])==0)
			break;
	}
	return i;
}

void print_prompt()
{
	uart_puts(promptStr);
}