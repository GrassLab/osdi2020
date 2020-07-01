#ifndef _SHELL_H
#define _SHELL_H

#include "uart.h"
#include "ftoa.h"
#include "mbox.h"
#include "loadimg.h"
#include "task.h"
#include "mm_allocator.h"
#include "vfs.h"

void reset(int tick);
void cancel_reset();
int strcmp (const char *p1, const char *p2);
char *strcpy(char *destination, const char *source);
int *strlen(const char *ch);
void *strcat(char *dest, const char *src);
void put_shell();

#endif
