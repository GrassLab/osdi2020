#include "types.h"
#include "string.h"
#include "time.h"
#include "mm.h"
#include "printf.h"
#include "uart.h"

uint64_t local_timer;
uint64_t core_timer;

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

float get_timestamp() {
    return mm_ticks()/(float)mm_freq();
}

void core_timer_enable() {
    asm volatile(
        "mov x0, #1                     \t\n\
        msr cntp_ctl_el0, x0            \t\n\
        mov x0, #2                      \t\n\
        ldr x1, =0x40000040             \t\n\
        str x0, [x1]"
    );
    core_timer = 0;
}

void core_timer_handler() {
    printf("Core timer interrupt %llu\r\n", core_timer);
    asm volatile(
        "mov x0, #0xFFFFFFF          \t\n\
        msr cntp_tval_el0, x0"
    );
    core_timer++;
}

void local_timer_init() {
    unsigned int flag = 0x30000000; // enable timer and interrupt.
    unsigned int reload = 3840000; // 2*19.2 MHz for 1 second delay
    local_timer = 0;
    mm_write(LOCAL_TIMER_CONTROL_REG, flag | reload);
    uart_log(LOG_WARNING, "Local timer initialized");
}

void local_timer_handler() {
    printf("Local timer interrupt %llu\r\n", local_timer);
    mm_write(LOCAL_TIMER_IRQ_CLR, 0xc0000000); // clear interrupt and reload.
    local_timer++;
}
