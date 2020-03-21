#include "include/gpio.h"
#include "include/uart.h"

#define SYSTMR_LO        ((volatile unsigned int*)(MMIO_BASE+0x00003004))
#define SYSTMR_HI        ((volatile unsigned int*)(MMIO_BASE+0x00003008))

/**
 * Wait N CPU cycles (ARM CPU only)
 */
void wait_cycles(unsigned int n)
{
    if(n) while(n--) { asm volatile("nop"); }
}

/**
 * Wait N microsec (ARM CPU only)
 */
void wait_msec(unsigned int n)
{
    register unsigned long f, t, r;
    // get the current counter frequency
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
    // read the current counter
    asm volatile ("mrs %0, cntpct_el0" : "=r"(t));
    // calculate expire value for counter
    t+=((f/1000)*n)/1000;
    do{asm volatile ("mrs %0, cntpct_el0" : "=r"(r));}while(r<t);
}

unsigned long get_timer_freq()
{
    register unsigned long f;
    // get the current counter frequency
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
    return f;
}

unsigned long get_timer_counter()
{
    register unsigned long t;
    // read the current counter
    asm volatile ("mrs %0, cntpct_el0" : "=r"(t));
    return t;
}


void get_time() 
{
    char timestamp[20];
    int i = 0; 
    int remain = 0;
    int quotient = 0;
    int mask = 100000;
    int c = 0;
    int precision = 5;
    unsigned long t, f;
    // read the current counter
    asm volatile ("mrs %0, cntpct_el0" : "=r"(t));
    // get the current counter frequency
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
    remain = t % f;

    timestamp[i++] = '[';
    quotient = t/f;
    if (quotient > mask) {
        uart_puts("timestamp larger than mask:100000\n");
    } else {

        while (mask > 0) {
            c = (quotient/mask) % 10;
            mask /= 10;
            
            timestamp[i++] = uart_i2c(c);
        }
        if (remain != 0) {

            timestamp[i++] = '.';
            
            while (precision > 0) {
                remain *= 10;
                timestamp[i++] = uart_i2c(remain/f);
                remain %= f;
                if (remain == 0) break;
                precision -= 1;
            }
            timestamp[i++] = ']';
            timestamp[i] = '\0';

            uart_puts(timestamp);
            uart_send('\n');
        }
    }
}
