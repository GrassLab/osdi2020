#include "time.h"
#include "io.h"
#include "gpio.h"
#include "uart.h"
#include "mbox.h"

void helper()
{
    uart_puts("---------------------");
    uart_puts("> help : show command list");
    uart_puts("> hello : print 'Hello World' on screen");
    uart_puts("> timestamp : print timestamp on screen");
    uart_puts("> reboot : reboot system");
    uart_puts( "> info : show board information" );
    uart_puts( "> loadimg : load new kernel from raspbootcom server" );
    uart_puts("---------------------");
}

void showMe()
{
    mbox[0] = 8*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
    mbox[2] = MBOX_TAG_GETSERIAL;   // get serial number command
    mbox[3] = 8;                    // buffer size
    mbox[4] = 8;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    // send the message to the GPU and receive answer
    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("My serial number is: ");
        uart_hex(mbox[6]);
        uart_hex(mbox[5]);
        uart_puts("\n");
    } else {
        uart_puts("Unable to query serial!\n");
    }

    mbox[0] = 8*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
    mbox[2] = MBOX_VC_BASE_MEM;   
    mbox[3] = 8;                    // buffer size
    mbox[4] = 8;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("VC BASE ADDRESS is: ");
        uart_hex(mbox[6]);
        uart_hex(mbox[5]);
        uart_puts("\n");
    } 

    mbox[0] = 8*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
    mbox[2] = MBOX_BOARD_RIVIESION;  
    mbox[3] = 8;                    // buffer size
    mbox[4] = 8;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("Board Rivision is: ");
        uart_hex(mbox[6]);
        uart_hex(mbox[5]);
        uart_puts("\n");
    } 
}

void loadimg(){
    
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
                uart_send('\r');
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
                }else if(!strCmp("info", buf)){
                    showMe();
                }else if(!strCmp("loadimg", buf)){
                    loadimg();
                }else{
                    uart_puts("Unknown command , please type 'help' to show!");
                }
                // clean buffer
                for(int i=0; i<10; i++){
                    buf[i] = '\0';
                }
                break;
            }
            // else if (a == 0x1B)
            // {
            //     input_c = uart_getc();
            //     if(input_c == 0x5B)
            //     {
            //         input_c = uart_getc();
            //         if(input_c == 0x43)
            //         {
            //             if(*(now+1)!='\0' && *now!='\0'){
            //                 uart_puts("\x1B\x5B\x43");
            //                 now++;
            //             }
            //         }
            //         if(input_c == 0x44)
            //         {
            //             if(*now != *str){
            //                 uart_puts("\x1B\x5B\x44");
            //                 now--;
            //             }
            //         }
            //     }
            // }
            // else if(a == 0x7f && i > 0) //backspace
            // {
            //     if(*now != 0)
            //     {
            //         uart_send(input_c);
            //         tmpc=now-1;
            //         while(*tmpc!=0)
            //         {
            //             *tmpc=*(tmpc+1);
            //             uart_send(*tmpc);
            //             tmpc++;
            //         }
            //         uart_send(' ');
            //         while(*tmpc--!=*now)
            //             uart_puts("\x1B\x5B\x44");
            //         uart_puts("\x1B\x5B\x44");
            //         if(*(now+1) == '\0')
            //             uart_puts("\x1B\x5B\x44");
            //         --now;
            //     }
            //     else if(count > 0){
            //         *(--now) = '\0';
            //         count--;
            //         uart_send(input_c);
            //     }
            //     continue;
            // }
            else{
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
