#include "mm.h"

#include "schedule.h"

static char kstack_pool[NR_TASKS][THREAD_SIZE];
static char ustack_pool[NR_TASKS][THREAD_SIZE];

unsigned long get_kstack_base(unsigned long task_id){
    return (unsigned long)(kstack_pool[task_id]);
}

unsigned long get_ustack_base(unsigned long task_id){
    return (unsigned long)(ustack_pool[task_id]);
}