#ifndef	_IRQ_H
#define	_IRQ_H

// src/irq.c
void enable_timer1_interrupt_controller(void);
void show_invalid_entry_message(unsigned long type, unsigned long esr, unsigned long address);
void handle_irq(void);
void system_call(unsigned long syscall_number, unsigned long x0, unsigned long x1);
void sync__handler(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3, unsigned long x4, unsigned long x5);
void irq_handler();

// src/irq.S
void irq_vector_init(void);
void enable_irq(void);
void disable_irq(void);
void svc_call_1(void);

#endif  /*_IRQ_H */
