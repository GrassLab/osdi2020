#include "../include/uart.h"
void main()
{
    int length = 0;
	//FLAG
	int FLAG_hello = 0;
	int FLAG_help = 0;
	int FLAG_reboot = 0;
	int FLAG_timestamp = 0;
	//get command
	int HELLO = 0;
	int HELP = 0;
	int REBOOT = 0;
	int TIMESTAMP = 0;
	//declare done

    // set up serial console
    uart_init();
    
    // welcome
    uart_puts("This is Lab1\n");
    
    // echo everything back
    for(;;)
    {
		//	char command[30];
        char key_in;
        key_in = uart_getc();
        uart_send(key_in);

		//process the command
		
		//hello
		if(key_in == 'h' && length == 0 && FLAG_hello == 0) FLAG_hello++;
		else if(key_in == 'e' && length == 1 && FLAG_hello == 1) FLAG_hello++;
		else if(key_in == 'l' && length == 2 && FLAG_hello == 2) FLAG_hello++;
		else if(key_in == 'l' && length == 3 && FLAG_hello == 3) FLAG_hello++;
		else if(key_in == 'o' && length == 4 && FLAG_hello == 4) HELLO = 1;
		else HELLO = 0;

		//help
		if(key_in == 'h' && length == 0 && FLAG_help == 0) FLAG_help++;
		else if(key_in == 'e' && length == 1 && FLAG_help == 1) FLAG_help++;
		else if(key_in == 'l' && length == 2 && FLAG_help == 2) FLAG_help++;
		else if(key_in == 'p' && length == 3 && FLAG_help == 3) HELP = 1;
		else HELP = 0;
		
		//reboot
		if(key_in == 'r' && length == 0 && FLAG_reboot == 0) FLAG_reboot++;
		else if(key_in == 'e' && length == 1 && FLAG_reboot == 1) FLAG_reboot++;
		else if(key_in == 'b' && length == 2 && FLAG_reboot == 2) FLAG_reboot++;
		else if(key_in == 'o' && length == 3 && FLAG_reboot == 3) FLAG_reboot++;
		else if(key_in == 'o' && length == 4 && FLAG_reboot == 4) FLAG_reboot++;
		else if(key_in == 't' && length == 5 && FLAG_reboot == 5) REBOOT = 1;
		else REBOOT = 0;
		
		//timestamp
		if(key_in == 't' && length == 0 && FLAG_timestamp == 0) FLAG_timestamp++;
		else if(key_in == 'i' && length == 1 && FLAG_timestamp == 1) FLAG_timestamp++;
		else if(key_in == 'm' && length == 2 && FLAG_timestamp == 2) FLAG_timestamp++;
		else if(key_in == 'e' && length == 3 && FLAG_timestamp == 3) FLAG_timestamp++;
		else if(key_in == 's' && length == 4 && FLAG_timestamp == 4) FLAG_timestamp++;
		else if(key_in == 't' && length == 5 && FLAG_timestamp == 5) FLAG_timestamp++;
		else if(key_in == 'a' && length == 6 && FLAG_timestamp == 6) FLAG_timestamp++;
		else if(key_in == 'm' && length == 7 && FLAG_timestamp == 7) FLAG_timestamp++;
		else if(key_in == 'p' && length == 8 && FLAG_timestamp == 8) TIMESTAMP = 1;
		else TIMESTAMP = 0;
		
		
        length++;
        if(key_in == '\n') 
		{
			if(HELLO) uart_puts("Hello World!\n");
			else if(HELP) uart_puts("hello : print Hello World!\nhelp : help\n reboot : reboot rpi3\ntimestamp : get current timestamp\n");
			else if(REBOOT) uart_puts("Let's Reboot!?\n");
			else if(TIMESTAMP) uart_puts("[206.XXXXXXX]?\n");
			else uart_puts("command not found, use help!!!\n");
			
			length = 0;
			
			//FLAG
			FLAG_hello = 0;
			FLAG_help = 0;
			FLAG_reboot = 0;
			FLAG_timestamp = 0;
			//get command
			HELLO = 0;
			HELP = 0;
			REBOOT = 0;
			TIMESTAMP = 0;
		}
        	
        	
        	

    }
}
