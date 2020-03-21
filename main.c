#include "include/uart.h"
#include "include/power.h"
#include "include/time.h"
#include "include/mailbox.h"


#define GET_BOARD_REVISION  0x00010002
#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000

void get_vccore_addr()
{
    return;
}

void get_board_revision()
{
    //unsigned int mailbox[7];
    mbox[0] = 8 * 4; // buffer size in bytes
    mbox[1] = REQUEST_CODE;
    // tags begin
    mbox[2] = GET_BOARD_REVISION; // tag identifier
    mbox[3] = 4; // maximum of request and response value buffer's length.
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0; // value buffer
    mbox[6] = 0;
    // tags end
    mbox[7] = END_TAG;

    //mailbox_call(mailbox); // message passing procedure call, you should implement it following the 6 steps provided above.
    //printf("0x%x\n", mailbox[5]); // it should be 0xa020d3 for rpi3 b+
    
    mbox_call(MBOX_CH_PROP);
    uart_puts("My board revision is: ");
    uart_hex(mbox[6]);
    uart_hex(mbox[5]);
    uart_puts("\n");
}

void get_serial()
{
    // get the board's unique serial number with a mailbox call
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
}

void main()
{
    // set up serial console
    uart_init();

    get_serial();
    get_board_revision();

    loop_start:do {
	
        char tmp;
        char user_input[12];
        int i = 0;
        uart_send('>');
        while (i < 10) {
            tmp = uart_getc();
            if (tmp == '\n') break;
            uart_send(tmp);
            user_input[i++] = tmp;
        }
        user_input[i] = '\0';
        uart_send('\n');
        //uart_send(uart_i2c(i));
        if (i == 0) {
            goto loop_start;
        }

        if (uart_strcmp(user_input, "hello") == 0) {
            uart_puts("Hello World!\n");
            goto loop_start;
        }
        
        if (uart_strcmp(user_input, "reboot") == 0) {
            get_time();
            uart_puts("reboot in 10 ticks.\n");
            reset(10);
            goto loop_start;
        }
        
        if (uart_strcmp(user_input, "time") == 0) {
            get_time();
            goto loop_start;    
        }
        
        if (uart_strcmp(user_input, "help") == 0) {
            uart_puts("hello: print hello world.\n");
            uart_puts("help: help.\n");
            uart_puts("reboot: restart.\n");
            uart_puts("time: show timestamp.\n");
            goto loop_start;
        }
        uart_puts("Error: command ");
        uart_puts(user_input);
        uart_puts(" not found, try <help>.\n");
    } while (1);
}
