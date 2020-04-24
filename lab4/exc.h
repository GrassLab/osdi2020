#ifndef __EXC_H__
#define __EXC_H__



void show_currentEL( void );
void sysCall_print_esr_elr( void );
void sysCall_set_timer( void );
void sysCall_unset_timer( void );
void sysCall_task_init( void );

#endif