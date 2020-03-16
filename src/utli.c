float get_timestamp() {
    register unsigned long f, c;
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(f)); // get current counter frequency
    asm volatile ("mrs %0, cntpct_el0" : "=r"(c)); // read current counter
    return (float) c / f;
}