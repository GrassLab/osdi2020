#pragma once

#include "gpio.h"


#define IRQ_BASIC_PENDING   ((volatile unsigned int*)(MMIO_BASE+0x0000B200))
#define IRQ_PENDING_1       ((volatile unsigned int*)(MMIO_BASE+0x0000B204))
#define IRQ_PENDING_2       ((volatile unsigned int*)(MMIO_BASE+0x0000B208))
#define FIQ_CONTROL         ((volatile unsigned int*)(MMIO_BASE+0x0000B20C))
#define ENABLE_IRQS_1       ((volatile unsigned int*)(MMIO_BASE+0x0000B210))
#define ENABLE_IRQS_2       ((volatile unsigned int*)(MMIO_BASE+0x0000B214))
#define ENABLE_BASIC_IRQS   ((volatile unsigned int*)(MMIO_BASE+0x0000B218))
#define DISABLE_IRQS_1      ((volatile unsigned int*)(MMIO_BASE+0x0000B21C))
#define DISABLE_IRQS_2      ((volatile unsigned int*)(MMIO_BASE+0x0000B220))
#define DISABLE_BASIC_IRQS  ((volatile unsigned int*)(MMIO_BASE+0x0000B224))

#define SYSTEM_TIMER_IRQ_0  (1 << 0)
#define SYSTEM_TIMER_IRQ_1  (1 << 1)
#define SYSTEM_TIMER_IRQ_2  (1 << 2)
#define SYSTEM_TIMER_IRQ_3  (1 << 3)

#define CORE0_IRQ_SRC       ((volatile unsigned int*)0x40000060)

#define AUX_IRQ             (1 << 29)

/* void enable_interrupt_controller( void ); */
void irq_handler();

/* irq.S */
void init_irq();
void disable_irq();
void enable_irq();
void irq_vector_init( void );
void enable_interrupt_controller();

/* irq.c */
void init_uart_irq();
