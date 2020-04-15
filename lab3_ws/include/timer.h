#ifndef _TIMER_INCLUDE_H_
#define _TIMER_INCLUDE_H_

void timmer_init();
float getCurrentTime();
unsigned long long getCurrentCount();
int getFrequency();
void local_timer_init();
#endif