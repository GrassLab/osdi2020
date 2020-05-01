#ifndef __SYS_SCHEDULE_H
#define __SYS_SCHEDULE_H

#include "task.h"

/* defined in schedule.S */
extern thread_info_t * get_current_task_el0 ( );
extern void set_next_task_el0 ( thread_info_t * );

void sys_do_schedule ( );
void sys_do_exec ( void ( *func ) ( ) );
void sys_do_exit ( thread_info_t * thread );

#endif