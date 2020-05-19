#ifndef TRAP_H
#define TRAP_H
#include "gpio.h"
#define CORE0_IRQ_SOURCE (volatile unsigned int *)0x40000060

#define ENABLE_IRQS_1 ((volatile unsigned int *)(MMIO_BASE + 0x0000B210))
#define AUX_IRQ (1 << 29)
#define IIR_REG_REC_NON_EMPTY (2 << 1)
void synchronize_handler();
void set_aux();
#endif
