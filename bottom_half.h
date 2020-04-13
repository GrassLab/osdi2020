#ifndef _BOTTOM_HALF_H_
#define _BOTTOM_HALF_H_

typedef struct bottom_half_t
{
    unsigned long num;
    void (*func)();
} bottom_half_t;

void bottom_half_enroll(bottom_half_t);

void bottom_half_set(unsigned long num);

void bottom_half_clr(unsigned long num);

void bottom_half_router();
#endif