#include "mm.h"

#include "schedule.h"

static char kstack_pool[NR_TASKS][THREAD_SIZE];

unsigned long get_kstack_base(unsigned long task_id){
    return (unsigned long)(kstack_pool[task_id] + THREAD_SIZE);
}