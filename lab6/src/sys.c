#include "fork.h"
#include "mini_uart.h"
#include "mm.h"
#include "printf.h"
#include "sched.h"
#include "utils.h"

void sys_write(char *buf) { printf(buf); }

int sys_fork() { return copy_process(0, 0, 0); }

void sys_exit() { exit_process(); }

char sys_read() { return uart_recv(); }

void sys_mem_usage() { stat_memory_usage(); }

int sys_get_taskid() { return current->pid; }

int sys_remain_page() { return remain_page; }

void *const sys_call_table[] = {sys_write,      sys_fork,      sys_exit,
                                sys_read,       sys_mem_usage, sys_get_taskid,
                                sys_remain_page};
