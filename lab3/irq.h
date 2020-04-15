#ifndef __IRQ_H__
#define __IRQ_H__

#define DISABLE_TIMER_COUNT 3
#define PI_DEFER_CYCLE 50000
#define QEMU_DEFER_CYCLE 7500000

void irq_int_enable(void);
void irq_int_disable(void);
void irq_el1_handler(void);

#endif

