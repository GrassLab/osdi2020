#ifndef _EXC_HANDLER_H
#define _EXC_HANDLER_H

#include "type.h"

void excHandler(uint64_t esr, uint64_t elr);

#endif