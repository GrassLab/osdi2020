#ifndef	_EXCEPTION_H
#define	_EXCEPTION_H
#include "base.h"

unsigned long get_syscall_number(unsigned long trapframe);
unsigned long get_syscall_parameter(unsigned long trapframe);
void exception_handler(unsigned long);

#endif