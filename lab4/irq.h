#ifndef __IRQ_H__
#define __IRQ_H__

void enable_interrupt( void );
void disable_interrupt( void );
void enable_EL0_interrupt( void );
void disable_EL0_interrupt( void );
void brk_instr( void );
void get_sync_exc_param_el1(unsigned long *esr, unsigned long *elr);
void get_sync_exc_param_el2(unsigned long *esr, unsigned long *elr);
void sleep( void );


int get_taskId( void );
void sched_yield( void );
void uart_write(char *s);
#endif