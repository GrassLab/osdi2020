#include"sched.h"
#include"uart.h"

#define S_FRAME_SIZE 272

void set_trap_ret(struct task* task_base, long long ret_val, int reg_offset){
    char *p;
    p = kstack_pool[task_base->taskid] - 16 - S_FRAME_SIZE ; //kstack_pool[current->taskid] is stack top!
    *((long long*)p + reg_offset) = ret_val; // 8byte per stack element
}

long get_trap_arg(int reg_offset){
    char *p;
    p = kstack_pool[current->taskid] - 16 - S_FRAME_SIZE ;
    return *((long*)p + reg_offset);
}