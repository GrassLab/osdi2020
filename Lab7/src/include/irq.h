#ifndef _IRQ_H
#define _IRQ_H

void irq_vector_init();
void enable_irq();
void disable_irq();
void async_exc_routing();

#endif /*_IRQ_H */
