#ifndef _TYPE_H
#define _TYPE_H

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