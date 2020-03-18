#include "uart.h"
#define input_buffer_Max 64
int strcmp(char *a, char *b)
{
	while(*a != '\0' || *b != '\n'){
		if(*a != *b) return -1;
		a++;
		b++;
	}
	return 0;
}
void help_cmd(void)
{
	uart_puts(" help  - print all available commands\n");
	uart_puts(" hello - print Hello World!\n");
}

void main()
{
	char s[input_buffer_Max];
	int i=0,j;

	uart_init();
		
	uart_puts("# ");
	while(1){
		if(i==input_buffer_Max || s[i-1]=='\n'){

			if(i==input_buffer_Max) uart_puts("\n Error command length! \n");
			else if(strcmp("help", s) == 0) 
				help_cmd();
			else if(strcmp("hello", s) == 0)
				uart_puts(" Hello World! \n");
			else{
				s[i-1]='\0';
				uart_puts(" Error:command '");
				uart_puts(s);
				uart_puts("' not found, try <help>\n");
			}
			for(j=0;j<i;j++){
			       	s[j]=0;
			}

			uart_puts("# ");
			i=0;	
		}
		uart_send( ( s[i++] = uart_getc() ) );
	}
}

