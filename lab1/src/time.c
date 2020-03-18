#include "io.h"
#define dps 100000000

void timestamp()
{
    register unsigned long time_FRQ;
    register unsigned long long time_CT;
    __asm__ volatile ("mrs %0, cntfrq_el0" : "=r"(time_FRQ));
    __asm__ volatile ("mrs %0, cntpct_el0" : "=r"(time_CT));
    unsigned long long count = time_CT,
                       freq = (unsigned long long)time_FRQ,
                       intp = count / freq, 
                       frap = count * dps / freq % dps;

    uart_puts("[");
    uart_puts(itoa((int)intp, 10));
    uart_puts(".");
    uart_puts(itoa((int)frap, 10));
    uart_puts("]");
}