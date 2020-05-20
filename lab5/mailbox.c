#include "gpio.h"

volatile unsigned int  __attribute__((aligned(16))) _mbox[36];

#define MAILBOX_BASE    (MMIO_BASE + 0xb880)

#define MAILBOX_READ    ((volatile unsigned int*)MAILBOX_BASE+0x0)
#define MAILBOX_STATUS  ((volatile unsigned int*)(MAILBOX_BASE + 0x18))
#define MAILBOX_WRITE   ((volatile unsigned int*)(MAILBOX_BASE + 0x20))
#define MBOX_RESPONSE   0x80000000
#define MAILBOX_EMPTY   0x40000000
#define MAILBOX_FULL    0x80000000
/*arm little*/
int mbox_call(unsigned char ch)
{
    unsigned int reg = (((unsigned int)((unsigned long)_mbox)&~0xF) | (ch&0xF));
    do{asm volatile("nop");}while(*MAILBOX_STATUS & MAILBOX_FULL);
    *MAILBOX_WRITE = reg;
    while(1){

        do{asm volatile("nop");}while(*MAILBOX_STATUS & MAILBOX_EMPTY);
        if(reg == *MAILBOX_READ)
        {
            return _mbox[1]==MBOX_RESPONSE;
        }
    }
    return 0;
}