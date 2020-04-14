#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "lib/type.h"

enum system_call {
    TEST_SVC = 1,
    TEST_HVC,
    IRQ_EL1_ENABLE,
    IRQ_EL1_DISABLE,
    CORE_TIMER_ENABLE,
    CORE_TIMER_DISABLE,
    PRINT_TIMESTAMP_EL0,
};

enum exception_level {
    EL0 = 0,
    EL1, EL2, EL3,
};

typedef enum system_call SYS_CALL;
typedef enum exception_level EL;


void exec_controller_el1 ( SYS_CALL x0 );
void print_exec_info ( EL el, uint64_t elr, uint64_t esr );

#endif