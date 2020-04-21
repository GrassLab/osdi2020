#ifndef	_IRQ_H
#define	_IRQ_H

// src/irq.c
void enable_timer1_interrupt_controller(void);
void test_handler_sync_invalid_el0_64(void);
void test_handler_irq_invalid_el0_64(void);
void sync_el0_64_handler(int x0, int x1, int x2, int x3, int x4, int x5);
void show_invalid_entry_message(int type, unsigned long esr, unsigned long address);
void handle_irq(void);
void system_call(unsigned int syscall_number);
void print_system_registers(void);

// src/irq.S
void irq_vector_init(void);
void enable_irq(void);
void disable_irq(void);
void svc_call_1(void);

#endif  /*_IRQ_H */
