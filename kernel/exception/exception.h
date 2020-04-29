#ifndef __EXCEPTION_H
#define __EXCEPTION_H

#include "lib/type.h"

#define SYS_CALL_TEST_SVC               "svc #1;"
#define SYS_CALL_TEST_HVC               "svc #2;"
#define SYS_CALL_IRQ_EL1_ENABLE         "svc #3;"
#define SYS_CALL_IRQ_EL1_DISABLE        "svc #4;"
#define SYS_CALL_CORE_TIMER_ENABLE      "svc #5;"
#define SYS_CALL_CORE_TIMER_DISABLE     "svc #6;"
#define SYS_CALL_PRINT_TIMESTAMP_EL0    "svc #7;"
#define SYS_CALL_SCHEDULE               "svc #8;"

#define HYPERVISORE_CALL_TEST_HVC       "hvc #2;"

#define LAUNCH_SYS_CALL(x) asm volatile ( x );  

// used by both sys call and hypervisor call
enum system_call {
    TEST_SVC = 1,
    TEST_HVC,
    IRQ_EL1_ENABLE,
    IRQ_EL1_DISABLE,
    CORE_TIMER_ENABLE,
    CORE_TIMER_DISABLE,
    PRINT_TIMESTAMP_EL0,
    SCHEDULE,
};

enum exception_level {
    EL0 = 0,
    EL1, EL2, EL3,
};

typedef enum system_call SYS_CALL;
typedef enum exception_level EL;

void exec_controller_el1 ( );
void exec_controller_el2 ( );
void print_exec_info ( EL el, uint64_t elr, uint64_t esr );

#endif