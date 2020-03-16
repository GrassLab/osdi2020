#include "../include/uart.h"
void main()
{
    int length = 0;
    int HELLO = 0;
    int FLAG_hello = 0;
    // set up serial console
    uart_init();
    
    // say hello
    uart_puts("This is Lab1\n");
    
    // echo everything back
    for(;;)
    {
//	char command[30];
        char key_in;
        key_in = uart_getc();
        uart_send(key_in);

	//process the command
	if(key_in == '\n' && FLAG_hello == 1) uart_puts("Hello World!\n");
	else if(key_in == 'h' && length == 0 && HELLO == 0) HELLO++;
	else if(key_in == 'e' && length == 1 && HELLO == 1) HELLO++;
	else if(key_in == 'l' && length == 2 && HELLO == 2) HELLO++;
	else if(key_in == 'l' && length == 3 && HELLO == 3) HELLO++;
	else if(key_in == 'o' && length == 4 && HELLO == 4) FLAG_hello = 1;
	else FLAG_hello = 0;

	
        length++;
        if(key_in == '\n') 
	{
	    length = 0;
	    FLAG_hello = 0;
	    HELLO = 0;
	}
        	
        	
        	
//	command[length] = key_in;

    }
}
