#ifndef __TASK_LIB
#define __TASK_LIB

/* defined in task.S */
extern void default_task_start ( );
extern void do_exec ( void(*func)() );
extern void do_exit ( );
extern int get_pid ( );

void idle ( );

#endif