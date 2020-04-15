#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include "mm.h"

enum {
    IRQ_BASIC_PENDING	= (MMIO_BASE+0x0000B200),
    IRQ_PENDING_1		= (MMIO_BASE+0x0000B204),
    IRQ_PENDING_2		= (MMIO_BASE+0x0000B208),
    FIQ_CONTROL		    = (MMIO_BASE+0x0000B20C),
    ENABLE_IRQS_1		= (MMIO_BASE+0x0000B210),
    ENABLE_IRQS_2		= (MMIO_BASE+0x0000B214),
    ENABLE_BASIC_IRQS	= (MMIO_BASE+0x0000B218),
    DISABLE_IRQS_1		= (MMIO_BASE+0x0000B21C),
    DISABLE_IRQS_2		= (MMIO_BASE+0x0000B220),
    DISABLE_BASIC_IRQS	= (MMIO_BASE+0x0000B224),

    CORE0_INT_SOURCE    = 0x40000060

};

void exception_handler(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3, unsigned long x4, unsigned long x5, unsigned long x8); 

void enable_interrupt_controller();

void handle_unknown();

void irq_handler();

void syscall(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3, unsigned long x4, unsigned long x5, unsigned long x8);

#endif
