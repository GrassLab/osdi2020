#ifndef IRQ_H
#define IRQ_H

#define CORE0_IRQ_SOURCE ((volatile unsigned int*)0x40000060)
#define CORE1_IRQ_SOURCE ((volatile unsigned int*)0x40000064)
#define CORE2_IRQ_SOURCE ((volatile unsigned int*)0x40000068)
#define CORE3_IRQ_SOURCE ((volatile unsigned int*)0x4000006C)

// define in irq.S
extern void irq_setup ();
extern void irq_enable ();
extern void irq_disable ();
extern void core_timer_enable ();
extern void core_timer_handler ();

void irq_controller ();

#endif