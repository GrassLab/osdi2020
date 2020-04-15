#include "io.h" 
#include "time.h" 

void wait_msec(unsigned int n)
{
    register unsigned long f, t, r;
    __asm__ volatile ("mrs %0, cntfrq_el0" : "=r"(f));
    __asm__ volatile ("mrs %0, cntpct_el0" : "=r"(t));
    t+=((f/1000)*n)/1000;
    do{__asm__ volatile ("mrs %0, cntpct_el0" : "=r"(r));}while(r<t);
}

void timestamp()
{
    register unsigned long f;
    register unsigned long long t;
    __asm__ volatile ("mrs %0, cntfrq_el0" : "=r"(f));
    __asm__ volatile ("mrs %0, cntpct_el0" : "=r"(t));
    unsigned long long count = t,
                       freq = (unsigned long long)f,
                       intp = count / freq, 
                       frap = count * dps / freq % dps;
    println("[", intp, ".", frap, "]");
}
