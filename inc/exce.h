#ifndef	_EXCE_H
#define	_EXCE_H

#include "peri_base.h"

// #define IRQ_BASIC_PENDING    ((volatile unsigned int*)(INTE_REG_BASE+0x200))
// #define IRQ_PENDING_1        ((volatile unsigned int*)(INTE_REG_BASE+0x204))
// #define IRQ_PENDING_2        ((volatile unsigned int*)(INTE_REG_BASE+0x208))
// #define FIQ_CONTROL          ((volatile unsigned int*)(INTE_REG_BASE+0x20C))
// #define ENABLE_IRQS_1        ((volatile unsigned int*)(INTE_REG_BASE+0x210))
// #define ENABLE_IRQS_2        ((volatile unsigned int*)(INTE_REG_BASE+0x214))
// #define ENABLE_BASIC_IRQS    ((volatile unsigned int*)(INTE_REG_BASE+0x218))
// #define DISABLE_IRQS_1       ((volatile unsigned int*)(INTE_REG_BASE+0x21C))
// #define DISABLE_IRQS_2       ((volatile unsigned int*)(INTE_REG_BASE+0x220))
// #define DISABLE_BASIC_IRQS   ((volatile unsigned int*)(INTE_REG_BASE+0x224))
#define CORE0_IRQ_SRC        ((volatile unsigned int*)0x40000060)


#define S_FRAME_SIZE         272 // size of all saved registers 
#define S_X0                 0   // offset of x0 register in saved stack frame

// #define IRQ_SYSTEM_TIMER_1	(1 << 1)
// #define IRQ_SYSTEM_TIMER_3	(1 << 3)

void sync_svc_handler(unsigned long esr, unsigned long elr);

// void enable_interrupt_controller();
// void handle_irq();
void enable_irq( void );
void disable_irq( void );
#endif  /*_EXC_H */