#include "sched.h"

#ifndef __MM_H__
#define __MM_H__

#define PAGE_SIZE       4096

#define INTR_STK_SIZE   PAGE_SIZE
#define KSTK_SIZE       PAGE_SIZE
#define USTK_SIZE       PAGE_SIZE

#endif

extern char *intr_stack;
extern char *kstack0; // kernel stack 0
extern char *ustack0;

void mm_init();
