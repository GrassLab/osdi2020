#include "type.h"

uint32_t getFrequency()
{
    uint32_t freq = 0;
    asm volatile("mrs %0, cntfrq_el0"
                 : "=r"(freq));
    return freq;
}

uint32_t getCount()
{
    uint32_t count = 0;
    asm volatile("mrs %0, cntpct_el0"
                 : "=r"(count));
    return count;
}

double getTime()
{
    double time = (double)getCount() / (double)getFrequency();
    return time;
}
