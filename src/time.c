#include "types.h"
#include "string.h"

void delay(int32_t count) {
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
            : "=r"(count): [count]"0"(count) : "cc");
}

static uint32_t mm_freq() {
    uint32_t cntfrq_el0;
    asm volatile("mrs %0, CNTFRQ_EL0" : "=r" (cntfrq_el0));
    return cntfrq_el0;
}

static uint64_t mm_ticks() {
    uint64_t cntpct_el0;
    asm volatile("mrs %0, CNTPCT_EL0" : "=r" (cntpct_el0));
    return cntpct_el0;
}

void get_timestamp(char* timestamp) {
     ftoa(mm_ticks()/(float)mm_freq(), timestamp);
}
