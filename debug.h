#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "printf.h"

#ifdef _DEBUG_TASK_
#define DEBUG_LOG_TASK(str) \
    printf str
#else
#define DEBUG_LOG_TASK(str) \
    while (0)               \
    {                       \
    };
#endif

#endif
