#ifndef __LIB_TASK_H
#define __LIB_TASK_H

/* defined in task.S */
extern void default_task_start ( );
extern void exec ( void ( *func ) ( ) );
extern void exit ( int );
extern int get_pid ( );
extern void sys_zombie_clearer ( );
extern int fork ( );

void idle ( );
void zombie_reaper ( );

#endif