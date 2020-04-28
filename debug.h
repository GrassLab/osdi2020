#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "printf.h"

// #define DEBUG_TASK
#define DEBUG_SYSCALL
// #define DEBUG_EXCEPTION
// #define DEBUG_SIGNAL

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
    init_printf(0, _putc);       \
    printf str;                  \
    init_printf(0, putc);
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

#ifdef DEBUG_SIGNAL
#define DEBUG_LOG_SIGNAL(str) \
    printf str
#else
#define DEBUG_LOG_SIGNAL(str) \
    while (0)                 \
    {                         \
    };
#endif

#endif
