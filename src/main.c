#include "../include/uart.h"
#include "../include/util.h"
#include "../include/reboot.h"
void main()
{
    int length = 0;

	//get command
	int HELLO = 0;
	int HELP = 0;
	int REBOOT = 0;
	int TIMESTAMP = 0;
	//declare done

    // set up serial console
    uart_init();

    uart_getc();
    // welcome
    uart_puts("\r\nWelcome to Lab1\n# ");
    int i;
	char input[100];
	for(i=0;i<100;i++) input[i] = '0';
    // echo everything back
    for(;;)
    {
        char key_in;
        key_in = uart_getc();
        uart_send(key_in);

		//compose the command
		if(key_in != '\n')
		{
			input[length] = key_in;
			length++;
		}
		
		//deal with the command
		
        if(key_in == '\n') 
		{
			//which command
			HELLO = StrCmp(input, "hello", length, 5);
			HELP = StrCmp(input, "help", length, 4);
			REBOOT  = StrCmp(input, "reboot", length, 6);
			TIMESTAMP = StrCmp(input, "timestamp", length, 9);
			//command detection done
			
			
			unsigned long long int time_counter, time_fre;
			
			asm volatile("mrs %0, CNTPCT_EL0" : "=r" (time_counter));
			asm volatile("mrs %0, CNTFRQ_EL0" : "=r" (time_fre));
			if(HELLO == 1) uart_puts("\rHello World!\n");
			else if(HELP) uart_puts("\rhello : print Hello World!\nhelp : help\nreboot : reboot rpi3\ntimestamp : get current timestamp\n");
			else if(REBOOT)
			{
				uart_puts(" \n\r");
				reset(3); 
			}
			else if(TIMESTAMP)
			{
				//print the timestamp
			    uart_puts("\r[ ");
			    Print_Int(time_counter);
			    uart_puts(" / ");
			    Print_Int(time_fre);
			    uart_puts(" ]\n");

			}
			else if(length != 0)
			{ 
				uart_puts("\rcommand  ");
				for(i=0;i<length;i++) uart_send(input[i]);
			        uart_puts("  not found, use help!!!\n");
			}
			
			
			if(REBOOT == 0) uart_puts("\r# ");
			

			//init input_command
			for(i=0;i<100;i++) input[i] = '0';
			length = 0;
			HELLO = 0;
			HELP = 0;
			REBOOT = 0;
			TIMESTAMP = 0;
		}
        	
        	
        	

    }
}
