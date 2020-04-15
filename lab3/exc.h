#include <stdint.h>
#include "uart.h"
#include "string_util.h"
#ifndef __EXC_H__
#define __EXC_H__

void exc_dispatcher(uint64_t identifier);
void exc_not_implemented(uint64_t code);
void exc_EL1_same_level_EL_SP_EL1_irq(void);
void exc_EL1_lower_aa64_EL_SP_EL1_sync(void);
void exc_EL1_lower_aa64_EL_SP_EL1_irq(void);
void exc_EL2_enable_physical_interrupt(void);

#endif

