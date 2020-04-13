#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "lib/type.h"

void exec_controller ( uint64_t esr, uint64_t elr );
int get_current_el ( );

#endif