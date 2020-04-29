#ifndef __SCHEDULE_H
#define __SCHEDULE_H

#include "task.h"

/* defined in schedule.S */
extern thread_info_t* get_current_task_el0 ();
extern void set_next_task_el0( thread_info_t * );

/* global variable for other file to use */
extern int RESCHED_FLAG;

void schedule ( );
void sys_do_exec ( void(*func)() );
void sys_do_exit ( thread_info_t * thread );

#endif