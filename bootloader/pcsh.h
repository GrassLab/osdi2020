#ifndef _PCSH_H_
#define _PCSH_H_
#include "common.h"

typedef struct cmd_t
{
    char *name;
    char *detail;
    int (*func)(int);
} cmd_t;

int cmd_exit(int);
int cmd_help(int);
int cmd_hello(int);
int cmd_reboot(int);
int cmd_timestamp(int);
int cmd_load_images(int);
int cmd_not_find(int);

int symbol();

int sh_default_command(char *);

int pcsh();

static cmd_t default_cmd_arr[];

#endif
