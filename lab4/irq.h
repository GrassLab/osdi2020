#ifndef __IRQ_H__
#define __IRQ_H__

void enable_interrupt( void );
void disable_interrupt( void );
void enable_EL0_interrupt( void );
void disable_EL0_interrupt( void );
void supervisor_call_1( void );
void supervisor_call_2( void );
void supervisor_call_3( void );
void supervisor_call_4( void );
void supervisor_call_5( void );
void supervisor_call_6( void );
void brk_instr( void );
void get_sync_exc_param_el1(unsigned long *esr, unsigned long *elr);
void get_sync_exc_param_el2(unsigned long *esr, unsigned long *elr);
void sleep( void );

#endif