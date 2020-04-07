#include "gpio.h"

#define MBOX_BASE    MMIO_BASE + 0xb880
#define MBOX_READ    ((volatile unsigned int*) (MBOX_BASE + 0x0))
#define MBOX_STATUS  ((volatile unsigned int*) (MBOX_BASE + 0x18))
#define MBOX_WRITE   ((volatile unsigned int*) (MBOX_BASE + 0x20))

#define MBOX_RESPONSE 0x80000000
#define MBOX_EMPTY   0x40000000
#define MBOX_FULL    0x80000000

/* mailbox message buffer */
volatile unsigned int  __attribute__((aligned(16))) mbox[36];

/**
 * Make a mailbox call. Returns 0 on failure, non-zero on success
 */

int mbox_call(unsigned char ch)
{
    /* 1. Combine the message address (upper 28 bits) with channel number (lower 4 bits) */
    unsigned int r = (((unsigned int)((unsigned long)&mbox)&~0xF) | (ch&0xF));
    /* 2. wait until we can write to the mailbox , check if Mailbox 0 status register’s full flag is set.*/
    do{asm volatile("nop");}while(*MBOX_STATUS & MBOX_FULL);
    /* 3. write the address of our message to the mailbox with channel identifier */
    *MBOX_WRITE = r;
    /* 4. is there a response? , check if Mailbox 0 status register’s empty flag is set.*/
    do{asm volatile("nop");}while(*MBOX_STATUS & MBOX_EMPTY);
    /* 5. is it a response to our message? , Check if the value is the same as you wrote in step 1.*/
    if(r == *MBOX_READ){
        /* is it a valid successful response? */
        return mbox[1]==MBOX_RESPONSE;
    }
    return 0;
}
