#ifndef TYPE_H
#define TYPE_H

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
    loadImg,
    none
} command_t;

#endif