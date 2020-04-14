#include "bottom_half.h"
#define BOTTOM_HALF_MAX_NUM 64

static unsigned long bottom_half_source = 0;

static bottom_half_t bottom_half_arr[BOTTOM_HALF_MAX_NUM];

void bottom_half_set(unsigned long num)
{
    bottom_half_source |= 1 << num;
}

void bottom_half_clr(unsigned long num)
{
    bottom_half_source &= ~(1 << num);
}

void bottom_half_enroll(bottom_half_t n)
{
    bottom_half_arr[n.num].func = n.func;
}

void bottom_half_router()
{
    if (bottom_half_source != 0)
    {
        for (int i = 0; i < BOTTOM_HALF_MAX_NUM; i++)
        {
            if ((1 << i & bottom_half_source) == 1)
            {
                bottom_half_arr[i].func();
                bottom_half_clr(i);
                break;
            }
        }
    }
}