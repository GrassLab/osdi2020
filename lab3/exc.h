#ifndef __EXC_H__
#define __EXC_H__

void enable_interrupt( void );
void disable_interrupt( void );
void show_currentEL( void );
void supervisor_call( void );
void brk_instr( void );

#endif