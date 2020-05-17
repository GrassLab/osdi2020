#ifndef _LIB_H_
#define _LIB_H_

double gettime();
void core_timer();
int fork();
int exec();
void exit();
int get_taskid();
void kill(int task_id, int signal);

void uart_send(unsigned int c);
char uart_recv();
#endif