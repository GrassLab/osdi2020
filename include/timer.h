#ifndef _TIMER_H
#define _TIMER_H

#include "task.h"
#include "uart.h"

void core_timer_enable();
void core_timer_handler();
void local_timer_enable();
void local_timer_handler();

#endif
