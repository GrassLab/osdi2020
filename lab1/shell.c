#include "uart.h"
#include "mystd.h"
#include "bcm2835_wdt.h"


#define CMDSIZE 64
char cmd[CMDSIZE] = {0};
int cmdSize = 0;

void cmd_process(){
	uart_puts("\r");

	if(strcmp(cmd, "hello")){
		uart_puts("Hello World! \n");
	}else if(strcmp(cmd, "help")){
		uart_puts("command: \"help\" Description: \"print all available commands\"  \n");
		uart_puts("command: \"hello\" Description: \"print Hello World!\"  \n");
		uart_puts("command: \"timestamp\" Description: \"print current timestamp\"  \n");
		uart_puts("command: \"reboot\" Description: \"reset raspi3\"  \n");
	}else if(strcmp(cmd, "timestamp")){
		unsigned long long cntfrq;
		unsigned long long cntpct;

		asm volatile(
			"mrs  %0, cntfrq_el0 \n"
			"mrs  %1, cntpct_el0 \n"
			: "=r"(cntfrq), "=r"(cntpct)
			::
		);

		char timeStr[65];
		cntTimeStamp(cntfrq, cntpct, timeStr);

		uart_puts("[");
		uart_puts(timeStr);
		uart_puts("]\n");
	}else if(strcmp(cmd, "reboot")){
		bcm2835_reboot(1000);
		uart_puts("\rreboot .... \n");
	}
	else if(strlen(cmd) != 0){
		uart_puts("command \"");
		uart_puts(cmd);
		uart_puts("\" not found, try <help> \n");	
	}

	uart_puts("# ");
}

void cmd_push(char c){
	if(cmdSize<CMDSIZE) {
		switch(c){
			case 8:		//backspace
				if(cmdSize>0) cmdSize--;
				uart_send(32);
				uart_send(8);
				break;
			case 10:	// 0X0A '\n' newline,  0X0D '\r' carriage return
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

	uart_puts("\rMy Simple Shell:\n# ");

	while(1){
		char c = uart_getc();

		if(c < 127){
			uart_send(c);		//show character user typed on uart terminal
			// printASCII(c);
			cmd_push(c);
		}

	}

}
