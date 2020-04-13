#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "lib/type.h"

enum system_call {
    TEST_SVC = 1,
    CORE_TIMER_ENABLE,
    CORE_TIMER_DISABLE,
};

typedef enum system_call SYS_CALL;


void exec_controller ( SYS_CALL x0 );
void print_exec_info ( uint64_t elr, uint64_t esr );
int get_current_el ( );

#endif