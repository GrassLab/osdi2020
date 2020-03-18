#include "time.h"
#include "io.h"
#include "gpio.h"
#include "uart.h"

void helper()
{
    uart_puts("---------------------");
    uart_puts("> help : show command list");
    uart_puts("> hello : print 'Hello World' on screen");
    uart_puts("> timestamp : print timestamp on screen");
    uart_puts("> reboot : reboot system");
    uart_puts("---------------------");
}

void main()
{
    // set up serial console
    uart_init();
    // Hello World when startup
    uart_puts("#     # ###### #      #       ####     #  #  #  ####  #####  #      #####  ");
    uart_puts("#     # #      #      #      #    #    #  #  # #    # #    # #      #    # ");
    uart_puts("####### #####  #      #      #    #    #  #  # #    # #    # #      #    # ");
    uart_puts("#     # #      #      #      #    #    #  #  # #    # #####  #      #    # ");
    uart_puts("#     # #      #      #      #    #    #  #  # #    # #   #  #      #    # ");
    uart_puts("#     # ###### ###### ######  ####      ## ##   ####  #    # ###### #####  \n\n");

    while(1) {
	    uart_send('#');
	    int i = 0;
	    char buf[10];
	    char a;
	    while(a = uart_getc()){
	    	if(a == '\n'){
                uart_send(a);
                if(!strCmp("hello", buf)){
                    uart_puts("Hello World!");
                }else if(!strCmp("help", buf)){
                    helper();
                }else if(!strCmp("timestamp", buf)){
                    timestamp();
                }else if (!strCmp("reboot", buf))
                {
                    reset();
                }else if(buf[0] == '\0'){
                    // ignore the new line case
                }else{
                    uart_puts("Unknown command , please type 'help' to show!");
                }
                // clean buffer
                for(int i=0; i<10; i++){
                    buf[i] = '\0';
                }
                break;
            }else{
                if( i == 10) // handle overflow
                {
                    uart_send(a);
                    continue;
                }
                uart_send(a);
                buf[i] = a;
                i=i+1;	
            }
	    }
    }
}
