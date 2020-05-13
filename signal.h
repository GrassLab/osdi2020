#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include "syscall.h"

#define SIGNAL_NUM 64

#define SIGINT 0
#define SIGKILL 1

void signal(int num, void (*func)());
void set_signal_handler(int taskid, int num, void (*func)());

void signal_raise(int taskid, int signal);

void signal_run(int taskid, int signal);

void signal_init();

void signal_router();

#endif