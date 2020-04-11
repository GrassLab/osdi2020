#include <stdint.h>
#include "uart.h"
#include "string_util.h"
#ifndef __EXEC_H__
#define __EXEC_H__

void exec_dispatcher(uint64_t identifier);
void exec_not_implemented(void);
void exec_EL2_current_EL_SP_EL2_sync(void);

#endif

