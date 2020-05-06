#ifndef __MM_H__
#define __MM_H__

#define KERNEL_BASE     0x80000

#define PAGE_SIZE       4096

#define KSTK_SIZE       PAGE_SIZE
#define USTK_SIZE       PAGE_SIZE

#endif

#ifndef IN_ASM // asm may also include this header file

#include "stdint.h"

struct stack_struct {
    char* top;
    uint64_t task_id;
    uint8_t avaliable;
};

extern struct stack_struct kstack_pool[];
extern struct stack_struct ustack_pool[];

void mm_init();
char* get_avaliable_ustack();
char* get_avaliable_kstack();
void release_ustack(int task_id);
void release_kstack(int task_id);

#endif
