#ifndef TRAP_H
#define TRAP_H

#include <stdint.h>
#include "gpio.h"
#define CORE0_IRQ_SOURCE (volatile unsigned int *)0x40000060
#define ENABLE_IRQS_1 ((volatile unsigned int *)(MMIO_BASE + 0x0000B210))
#define AUX_IRQ (1 << 29)
#define IIR_REG_REC_NON_EMPTY (2 << 1)

struct trap_frame_t {
    uint64_t x30;
    uint64_t x29;
    uint64_t x28;
    uint64_t x27;
    uint64_t x26;
    uint64_t x25;
    uint64_t x24;
    uint64_t x23;
    uint64_t x22;
    uint64_t x21;
    uint64_t x20;
    uint64_t x19;
    uint64_t x18;
    uint64_t x17;
    uint64_t x16;
    uint64_t x15;
    uint64_t x14;
    uint64_t x13;
    uint64_t x12;
    uint64_t x11;
    uint64_t x10;
    uint64_t x9;
    uint64_t x8;
    uint64_t x7;
    uint64_t x6;
    uint64_t x5;
    uint64_t x4;
    uint64_t x3;
    uint64_t x2;
    uint64_t x1;
    uint64_t x0;
};
void synchronize_handler();
void set_aux();
#endif
