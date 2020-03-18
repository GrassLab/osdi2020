/*
 * mm.h
 * Provides a basic API to interact with rpi3's GPIOs
 * 
 * source: https://jsandler18.github.io/tutorial/boot.html
 */

#ifndef __MM_H_
#define __MM_H_

#define MMIO_BASE        0x3F000000

#define GPFSEL1          ((volatile unsigned int*)(MMIO_BASE+0x00200004))
#define GPSET0           ((volatile unsigned int*)(MMIO_BASE+0x0020001C))
#define GPCLR0           ((volatile unsigned int*)(MMIO_BASE+0x00200028))
#define GPPUD            ((volatile unsigned int*)(MMIO_BASE+0x00200094))
#define GPPUDCLK0        ((volatile unsigned int*)(MMIO_BASE+0x00200098))

#include "types.h"


static inline void mm_write(volatile uint32_t *reg, uint32_t data) {
    *(reg) = data;
}

static inline uint32_t mm_read(volatile uint32_t *reg) {
    return *(reg);
}

static inline void delay(int32_t count) {
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
            : "=r"(count): [count]"0"(count) : "cc");
}

static inline uint32_t mm_freq() {
    uint32_t cntfrq_el0;

    asm volatile("mrs %0, CNTFRQ_EL0\n" : "=r" (cntfrq_el0) : : "memory");
    return cntfrq_el0;
}

static inline uint64_t mm_ticks() {
    uint64_t cntpct_el0;

    asm volatile("mrs %0, cntpct_el0\n" : "=r" (cntpct_el0) : : "memory");
    return cntpct_el0;
}

#endif
