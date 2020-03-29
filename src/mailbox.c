#include "gpio.h"
#include "mailbox.h"
#include "mini_uart.h"
#include "utils.h"

/* mailbox message buffer */
volatile unsigned int  __attribute__((aligned(16))) mbox[36];

#define MBOX_REG_BASE  (MMIO_BASE+0x0000B880)
static struct _MBOX_REG
{
    volatile unsigned int READ;      // +0x00
    volatile unsigned int RESERVED0; // +0x04
    volatile unsigned int RESERVED1; // +0x08
    volatile unsigned int RESERVED2; // +0x0C
    volatile unsigned int POLL;      // +0x10
    volatile unsigned int SENDER;    // +0x14
    volatile unsigned int STATUS;    // +0x18
    volatile unsigned int CONFIG;    // +0x1C
    volatile unsigned int WRITE;     // +0x20
} *MBOX_REG = (void *)MBOX_REG_BASE;


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

void init_mbox_buff(unsigned int tag_id){
    // header
    mbox[0] = 8*4;                  // buffer size in bytes
    mbox[1] = MBOX_REQUEST_CODE;    // buffer request/response code
    // tag
    mbox[2] = tag_id;               // tag identifier
    mbox[3] = 5;                    // buffer size
    mbox[4] = MBOX_TAG_REQUEST_CODE;// tag request/response code
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;                    // clear output buffer
    mbox[7] = 0;                    // clear output buffer
    mbox[8] = MBOX_END_TAG;
}

void get_board_revision(){
    char buff[4];
    init_mbox_buff(MBOX_TAG_GET_BREVI);
    if (mbox_call(MBOX_CH_PROPT_ARM_VC)) {
        uart_puts("Board revision: ");
        bin2hex(mbox[5], buff);
        uart_puts(buff);
        uart_puts("\n");
    } else {
        uart_puts("Unable to query board revision!\n");
    }
}

void get_ARM_address(){
    char buff[4];
    init_mbox_buff(MBOX_TAG_GET_ARMADDR);
    if (mbox_call(MBOX_CH_PROPT_ARM_VC)){
        uart_puts("ARM address base: ");
        bin2hex(mbox[5], buff);
        uart_puts(buff);
        uart_puts("\tsize: ");
        bin2hex(mbox[6], buff);
        uart_puts(buff);
        uart_puts("\n");
    } else {
        uart_puts("Unable to query ARM address!\n");
    }
}

void get_VC_address(){
    char buff[4];
    init_mbox_buff(MBOX_TAG_GET_VCADDR);
    if (mbox_call(MBOX_CH_PROPT_ARM_VC)){
        uart_puts("VC address base: ");
        bin2hex(mbox[5], buff);
        uart_puts(buff);
        uart_puts("\tsize: ");
        bin2hex(mbox[6], buff);
        uart_puts(buff);
        uart_puts("\n");
    } else {
        uart_puts("Unable to query VC address!\n");
    }
}