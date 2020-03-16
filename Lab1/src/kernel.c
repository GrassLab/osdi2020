#include "include/mini_uart.h"
#include "include/utils.h"
#include "include/reboot.h"

void kernel_main(void)
{
    uart_init();
    uart_send_string("Hello, world!\r\n");

    // simple shell implement
    char str[128];
    char i=0;
    int check = 0;
    char recv_char;

    *(str) = '\0';
    uart_send_string(">>");
    while (1) {
	recv_char = uart_recv();
    	uart_send(recv_char);
	
	if(recv_char>32 && recv_char<127){
		*(str+i)=recv_char;
		i++;
	}

	else if  (recv_char =='\n' || recv_char == '\r'){
		*(str+i) = '\0';		
		check = uart_check_string(str); 
		
		if (check ==0){	
			i=0;
			*(str) = '\0'; //reset what it will output
			uart_send_string(">>");
		}	

	}			
	 
    }
}
