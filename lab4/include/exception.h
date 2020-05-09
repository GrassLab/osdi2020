#ifndef	_EXCEPTION_H
#define	_EXCEPTION_H
#include "base.h"

unsigned int get_syscall_number(unsigned int trapframe);
unsigned int get_syscall_parameter(unsigned int trapframe);
void exception_handler(unsigned int);

#endif