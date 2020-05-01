#ifndef __SYS_EXCEPTION_H
#define __SYS_EXCEPTION_H

#include "lib/type.h"

#include "sys_call_def.h"

// used by both sys call and hypervisor call
enum exception_level
{
    EL0 = 0,
    EL1,
    EL2,
    EL3,
};

typedef enum system_call SYS_CALL;
typedef enum exception_level EL;

void exec_controller_el1 ( );
void print_exec_info ( EL el, uint64_t elr, uint64_t esr );

#endif