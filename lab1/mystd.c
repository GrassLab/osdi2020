#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int*)(0x3F10001c))
#define PM_WDOG ((volatile unsigned int*)(0x3F100024))

int strcmp(char *s1, char *s2){
    while(*s1 || *s2){
        if(*s1 != *s2)
            return 0;
        ++s1; ++s2;
    }
    return 1;
}

double get_time(){
    register double frequency, counter;
    double time;
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(frequency));
    asm volatile ("mrs %0, cntpct_el0" : "=r"(counter));
    time = counter / frequency;
    return time;
}

void reset(int tick){ // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20; // full reset
  *PM_WDOG = PM_PASSWORD | tick; // number of watchdog tick
}

void cancel_reset(){
  *PM_RSTC = PM_PASSWORD | 0; // full reset
  *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}