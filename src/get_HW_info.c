#include "../include/mbox.h"
#include "../include/uart.h"
#include "../include/gpio.h"


void Get_Serial_number()
{
    uart_init();

    mbox[0] = 8 *4;
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
    mbox[2] = MBOX_TAG_GETSERIAL;   // get serial number command
    mbox[3] = 8;                    // buffer size
    mbox[4] = 8;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    // send the message to the GPU and receive answer

    // get serial number
    if(mbox_call(MBOX_CH_PROP)) 
    {
        uart_puts("My serial number is: ");
        uart_hex(mbox[5]);
        uart_hex(mbox[6]);
        uart_puts("\n");
    } 
    else uart_puts("Unable to query serial!\n");
    
}


void Get_Board_revision()
{
    uart_init();
    mbox[0] = 7 * 4;                       // length of the message
    mbox[1] = MBOX_REQUEST;                // this is a request message 
    mbox[2] = MBOX_TAG_GETBOARDREVISION;   // get board revision number command
    mbox[3] = 4;                           // buffer size
    mbox[4] = 8;
    mbox[5] = 0;                           // clear output buffer
    mbox[6] = MBOX_TAG_LAST;

    if(mbox_call(MBOX_CH_PROP)) 
    {
        uart_puts("My board revision is: ");
        uart_hex(mbox[5]);
        uart_puts("\n");
    } 
    else uart_puts("Unable to query board revision!\n");
    
}

void Get_VC_mem()
{
    uart_init();
    mbox[0] = 8*4;                        // length of the message
    mbox[1] = MBOX_REQUEST;               // this is a request message
    mbox[2] = MBOX_TAG_GETVCMEMORYADDR;   // get VC memory base address command
    mbox[3] = 8;                          // buffer size
    mbox[4] = 8;
    mbox[5] = 0;                          // clear output buffer
    mbox[6] = 0;
    mbox[7] = MBOX_TAG_LAST;

    if(mbox_call(MBOX_CH_PROP)) 
    {
        uart_puts("My VC memory base address is: ");
        uart_hex(mbox[5]);
        uart_puts("\n");
        uart_puts("My VC memory size is: ");
        uart_hex(mbox[6]);
        uart_puts("\n");
    } 
    else uart_puts("Unable to query serial!\n");
    
}













