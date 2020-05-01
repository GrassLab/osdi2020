#ifndef __LIB_TASK_H
#define __LIB_TASK_H

/* defined in task.S */
extern void default_task_start ( );
extern void exec ( void ( *func ) ( ) );
extern int get_pid ( );

void idle ( );

#endif