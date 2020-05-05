#include"sched.h"

#define S_FRAME_SIZE 272

void set_trap_ret(struct task* task_base, int ret_val){
    char *p;
    p = kstack_pool[task_base->taskid] - 16 - S_FRAME_SIZE ; //kstack_pool[current->taskid] is stack top!
    *((long long*)p) = ret_val; // 8byte per stack element
}