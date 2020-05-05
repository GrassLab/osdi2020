#ifndef _SWITCH_H
#define _SWITCH_H

#include "type.h"
#include "task/taskStruct.h"

void switchTo(struct cpu_context *next);
void copyAndSwitchTo(struct cpu_context *prev, struct cpu_context *next);
void switchToEL0(struct cpu_context *cur);
void copyStack(uint64_t dst, uint64_t source, uint32_t size);
void copyContext(struct cpu_context *dst);

#endif