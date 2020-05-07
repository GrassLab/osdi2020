#ifndef __SYS_TIME_H
#define __SYS_TIME_H

double sys_get_current_timestamp ( );
void sys_wait_cycles ( unsigned int n );
void sys_wait_msec ( unsigned int n );

#endif