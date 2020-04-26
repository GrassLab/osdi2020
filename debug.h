#ifndef _DEBUG_H_
#define _DEBUG_H_

// #define DEBUG_TASK
// #define DEBUG_SYSCALL
#define DEBUG_EXCEPTION

#include "printf.h"

#ifdef DEBUG_TASK
#define DEBUG_LOG_TASK(str) \
    printf str
#else
#define DEBUG_LOG_TASK(str) \
    while (0)               \
    {                       \
    };
#endif

#ifdef DEBUG_EXCEPTION
#define DEBUG_LOG_EXCEPTION(str) \
    printf str
#else
#define DEBUG_LOG_EXCEPTION(str) \
    while (0)                    \
    {                            \
    };
#endif

#ifdef DEBUG_SYSCALL
#define DEBUG_LOG_SYSCALL(str) \
    printf str
#else
#define DEBUG_LOG_SYSCALL(str) \
    while (0)                  \
    {                          \
    };
#endif

#endif
