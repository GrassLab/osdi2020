#include <stdint.h>
#include "uart.h"
#include "string_util.h"
#ifndef __EXC_H__
#define __EXC_H__

void exc_dispatcher(uint64_t identifier);
void exc_not_implemented(void);
void exc_EL2_current_EL_SP_EL2_sync(void);

#endif

