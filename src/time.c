#include "time.h"

unsigned int getFrequency() {
    unsigned int freq = 0;
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(freq));
    return freq;
}

unsigned int getCount() {
    unsigned int count = 0;
    asm volatile ("mrs %0, cntpct_el0" : "=r"(count));
    return count;
}

double getTime() {
    double time = (double)getCount() / (double)getFrequency();
    return time;
}

