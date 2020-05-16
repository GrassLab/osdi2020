#include "typedef.h"

#ifndef __EXC_H__
#define __EXC_H__

#define INTR_STK_SIZE 4096

struct trapframe {
    uint64_t x[31]; // general register from x0 ~ x30
    uint64_t sp_el0;
    uint64_t elr_el1;
    uint64_t spsr_el1;
};

#endif

/* Variables init in exception.c */
extern char intr_stack[INTR_STK_SIZE];
extern uint64_t arm_core_timer_jiffies, arm_local_timer_jiffies;
extern uint64_t cntfrq_el0, cntpct_el0;

/* Function in exception_table.S */
extern void return_from_fork();

/* Function in exception.c */
void irq_enable();
void irq_disable();
void arm_core_timer_enable();
void arm_core_timer_disable();
void arm_local_timer_enable();
void arm_local_timer_disable();