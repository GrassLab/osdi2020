#ifndef	_IRQ_H
#define	_IRQ_H
void handle_irq(unsigned long esr, unsigned long address);
void handle_sys_timer_irq();
void handle_core_timer_irq(); 
void handle_el1_irq();
#endif