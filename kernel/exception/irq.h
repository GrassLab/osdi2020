#ifndef __IRQ_H
#define __IRQ_H

#define CORE0_IRQ_SOURCE ((volatile unsigned int*)0x40000060)
#define CORE1_IRQ_SOURCE ((volatile unsigned int*)0x40000064)
#define CORE2_IRQ_SOURCE ((volatile unsigned int*)0x40000068)
#define CORE3_IRQ_SOURCE ((volatile unsigned int*)0x4000006C)

// define in irq.S
extern void irq_el1_enable ( );
extern void irq_el1_disable ( );
extern void irq_el2_setup ( );
extern void irq_el2_enable ( );
extern void irq_el2_disable ( );

void irq_controller_el1 ( );
void irq_controller_el2 ( );

#endif