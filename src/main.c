#include "../include/uart.h"
#include "../include/reboot.h"
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
		else if(key_in != '\n') HELLO = 0;

		//help
		if(key_in == 'h' && length == 0 && FLAG_help == 0) FLAG_help++;
		else if(key_in == 'e' && length == 1 && FLAG_help == 1) FLAG_help++;
		else if(key_in == 'l' && length == 2 && FLAG_help == 2) FLAG_help++;
		else if(key_in == 'p' && length == 3 && FLAG_help == 3) HELP = 1;
		else if(key_in != '\n') HELP = 0;
		
		//reboot
		if(key_in == 'r' && length == 0 && FLAG_reboot == 0) FLAG_reboot++;
		else if(key_in == 'e' && length == 1 && FLAG_reboot == 1) FLAG_reboot++;
		else if(key_in == 'b' && length == 2 && FLAG_reboot == 2) FLAG_reboot++;
		else if(key_in == 'o' && length == 3 && FLAG_reboot == 3) FLAG_reboot++;
		else if(key_in == 'o' && length == 4 && FLAG_reboot == 4) FLAG_reboot++;
		else if(key_in == 't' && length == 5 && FLAG_reboot == 5) REBOOT = 1;
		else if(key_in != '\n') REBOOT = 0;
		
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
		else if(key_in != '\n') TIMESTAMP = 0;
		
		
        length++;
        if(key_in == '\n') 
		{
			char count[30];
			char fre[30];
			int FLAG_count = 0, FLAG_fre = 0;
			int time_counter, time_fre;
			int i;
			asm volatile("mrs %0, CNTPCT_EL0" : "=r" (time_counter));
			asm volatile("mrs %0, CNTFRQ_EL0" : "=r" (time_fre));
			if(HELLO == 1) uart_puts("\rHello World!\n");
			else if(HELP) uart_puts("\rhello : print Hello World!\nhelp : help\nreboot : reboot rpi3\ntimestamp : get current timestamp\n");
			else if(REBOOT) reset(10) 
			else if(TIMESTAMP)
			{
			    uart_puts("\r[ ");
			    //init
			    for(i=0;i<30;i++) count[i] = '0';
			    for(i=0;i<30;i++) fre[i] = '0';
			    char num;
			    i = 29;
			    while(time_counter > 0)
			    {
				    num = (time_counter % 10) + '0';
				    time_counter = time_counter / 10;
				    count[i--] = num;

			    }
			    i = 29;
			    while(time_fre > 0)
			    {
				    num = (time_fre % 10) + '0';
				    time_fre = time_fre / 10;
				    fre[i--] = num;
			    }
			    for(i=0;i<30;i++)
			    {
				    if(FLAG_count == 0)
				    {
					    if(count[i] == '0') continue;
					    else 
					    {
						    FLAG_count = 1;
						    uart_send(count[i]);
					    }
				    }
				    else uart_send(count[i]);
			    }
			    uart_puts(" / ");
			    for(i=0;i<30;i++)
			    {
				    if(FLAG_fre == 0)
				    {
					    if(fre[i] == '0') continue;
					    else
					    {
						    FLAG_fre = 1;
						    uart_send(count[i]);
					    }
				    }
				    else uart_send(count[i]);
			    }
			    uart_puts(" ]\n");

			}
			else if(length != 1)uart_puts("\rcommand not found, use help!!!\n");
			
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
