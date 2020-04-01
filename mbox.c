#include "mbox.h"
#include "uart.h"

#define MMIO_BASE		0x3F000000
#define MAILBOX_BASE    (MMIO_BASE + 0xb880)

#define MAILBOX_READ    ((volatile unsigned int*)MAILBOX_BASE+0x0)
#define MAILBOX_STATUS  ((volatile unsigned int*)(MAILBOX_BASE + 0x18))
#define MAILBOX_WRITE   ((volatile unsigned int*)(MAILBOX_BASE + 0x20))

#define MBOX_RESPONSE   0x80000000
#define MAILBOX_EMPTY   0x40000000
#define MAILBOX_FULL    0x80000000

int mailbox_call(unsigned int *m, unsigned char ch)
{
    unsigned int r = (((unsigned int)((unsigned long)m)&~0xF) | (ch&0xF));
    do{asm volatile("nop");}while(*MAILBOX_STATUS & MAILBOX_FULL);
    *MAILBOX_WRITE = r;
    while(1){
        do{asm volatile("nop");}while(*MAILBOX_STATUS & MAILBOX_EMPTY);
		
		if(r == *MAILBOX_READ){
            return m[1]==MBOX_RESPONSE;
        }
			    }
    return 0;
}
