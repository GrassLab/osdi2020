#ifndef	_EXCE_H
#define	_EXCE_H

#include "base.h"

// #define IRQ_SYSTEM_TIMER_1	(1 << 1)
// #define IRQ_SYSTEM_TIMER_3	(1 << 3)

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

#define IRQ_SRC_CNTPS           (1 << 0)
#define IRQ_SRC_CNTPNS          (1 << 1)
#define IRQ_SRC_CNTHP           (1 << 2)
#define IRQ_SRC_CNTV            (1 << 3)
#define IRQ_SRC_MBOX_0          (1 << 4)
#define IRQ_SRC_MBOX_1          (1 << 5)
#define IRQ_SRC_MBOX_2          (1 << 6)
#define IRQ_SRC_MBOX_3          (1 << 7)
#define IRQ_SRC_GPU             (1 << 8)
#define IRQ_SRC_PMU             (1 << 9)
#define IRQ_SRC_AXI             (1 << 10)
#define IRQ_SRC_LOCAL_TIMER     (1 << 11)

#ifndef __ASSEMBLER__
struct trapframe {
    unsigned long Xn[29]; // general register
    unsigned long fp;
    unsigned long lr;
    unsigned long sp_el0;
    unsigned long elr_el1;
    unsigned long spsr_el1;
};

// void enable_interrupt_controller();
// void handle_irq();
void irq_vector_init();
void enable_irq();
void disable_irq();
void ret_fork_child();
#endif//__ASSEMBLER__

#endif  /*_EXC_H */