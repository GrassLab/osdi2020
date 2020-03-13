#include "uart.h"
#include "mystd.h"

#define CMDSIZE 64

char cmd[64] = {0};
int cmdSize = 0;

void cmd_process(){

	if(strcmp(cmd, "hello")){
		uart_puts("Hello World! \n");
	}else if(strcmp(cmd, "help")){
		uart_puts("command: \"help\" Description: \"print all available commands\"  \n");
		uart_puts("command: \"hello\" Description: \"print Hello World!\"  \n");
	}
	else{
		uart_puts("command \"");
		uart_puts(cmd);
		uart_puts("\" doesn't exist \n");	
	}

	uart_puts("# ");
}

void cmd_push(char c){
	if(cmdSize<CMDSIZE) {
		switch(c){
			case 8:		//backspace
				if(cmdSize>0) cmdSize--;
				break;
			case 10:	//enter
				cmd[cmdSize] = '\0';
				cmdSize++;
				cmd_process();
				cmdSize = 0;
				break;

			default :
				cmd[cmdSize] = c;
				cmdSize++;	
		}
	}
}

void main()
{
	uart_init();

	uart_puts("My Simple Shell:\n# ");

	while(1){
		char c = uart_getc();

		uart_send(c);		//show character user typed on uart terminal
		cmd_push(c);

	}

}
