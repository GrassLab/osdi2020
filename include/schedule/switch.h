#ifndef _SWITCH_H
#define _SWITCH_H

#include "schedule/task.h"

void switchTo();
void switchToEL0();
struct task* getCurrent();

#endif