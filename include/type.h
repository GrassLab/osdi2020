#ifndef _TYPE_H
#define _TYPE_H

typedef int int32_t;
typedef long int64_t;

typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

typedef unsigned int size_t;

typedef enum
{
    false = 0,
    true = 1
} bool;

typedef enum
{
    help,
    hello,
    timestamp,
    reboot,
    hardwareInfo,
    exc,
    irq,
    none
} command_t;

#endif