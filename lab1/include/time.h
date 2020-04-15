#ifndef __TIME_H__
#define __TIME_H__

#define sleep(n) wait_msec(n * 1000000)
void wait_msec(unsigned int n);
#define dps 100000000
void timestamp();

#endif
