#include "mailbox.h"
#include "string.h"
#include "uart.h"
#include "printf.h"


/* mailbox message buffer */
volatile unsigned int  __attribute__((aligned(16))) mbox[36];


/**
 * Make a mailbox call. Returns 0 on failure, non-zero on success
 */
int mbox_call(unsigned char ch)
{
    unsigned int r = (((unsigned int)((unsigned long)&mbox)&~MBOX_CH_MASK) | (ch&MBOX_CH_MASK));
    /* wait until we can write to the mailbox */
    do{
        asm volatile("nop");
    }while(MBOX_REG-> STATUS & MBOX_FULL);
    /* write the address of our message to the mailbox with channel identifier */
    MBOX_REG-> WRITE = r;
    /* now wait for the response */
    while(1) {
        /* is there a response? */
        do{
            asm volatile("nop");
        }while(MBOX_REG-> STATUS & MBOX_EMPTY);
        /* is it a response to our message? */
        if(r == MBOX_REG-> READ)
            return mbox[1]==MBOX_RESPONSE_SUCCESS;
    }
    return 0;
}

void get_board_info(){
    // header
    mbox[ 0] = 17*4;                  // buffer size in bytes
    mbox[ 1] = MBOX_REQUEST_CODE;     // buffer request/response code
    // tags
    mbox[ 2] = MBOX_TAG_GET_BREVI;    // tag identifier
    mbox[ 3] = 1*4;                   // value buffer size in bytes
    mbox[ 4] = MBOX_TAG_REQUEST_CODE; // tag request/response code
    mbox[ 5] = 0;                     // value buffer

    mbox[ 6] = MBOX_TAG_GET_ARMADDR;
    mbox[ 7] = 2*4;
    mbox[ 8] = MBOX_TAG_REQUEST_CODE;
    mbox[ 9] = 0;
    mbox[10] = 0;

    mbox[11] = MBOX_TAG_GET_VCADDR;
    mbox[12] = 2*4;
    mbox[13] = MBOX_TAG_REQUEST_CODE;
    mbox[14] = 0;
    mbox[15] = 0;
    // tailer
    mbox[16] = MBOX_END_TAG;

    // send the message to the GPU and receive answer
    if (mbox_call(MBOX_CH_PROPT_ARM_VC)) {
        // printf("Board revision: 0x%08X\n", mbox[5]);
        // printf("ARM address base: 0x%08X\tsize: 0x%08X\n", mbox[9], mbox[10]);
        // printf("VC  address base: 0x%08X\tsize: 0x%08X\n", mbox[14], mbox[15]);
        uart_puts("Board revision:");
        uart_hex(mbox[5]);
        uart_puts("\nARM address base: ");
        uart_hex(mbox[9]);
        uart_puts("\tsize: ");
        uart_hex(mbox[10]);
        uart_puts("\nVC  address base: ");
        uart_hex(mbox[14]);
        uart_puts("\tsize: ");
        uart_hex(mbox[15]);
        uart_puts("\n");
    } else {
        uart_puts("Mailbox fail to query board info!\n");
    }
}