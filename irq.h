#ifndef _IRQ_H
#define _IRQ_H

#define CORE0_INTR_SRC (volatile unsigned int *)0x40000060
#define IRQ_BASIC_PENDING (volatile unsigned int *)(MMIO_BASE + 0xb200)
#define ENABLE_IRQ2 (volatile unsigned int *)(MMIO_BASE + 0xb214)

extern void _irq_init();
extern void _irq_end();

extern void irq();
#endif