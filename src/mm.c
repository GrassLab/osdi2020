#include "mm.h"

#include "sched.h"

// static char kstack_pool[NR_TASKS][THREAD_SIZE];
static unsigned short kstack_map [NR_TASKS] = {0,};

unsigned long get_free_kstack(){
    for (int i=0; i<NR_TASKS; i++){
        if (kstack_map[i] == 0){
            kstack_map[i] = 1;
            return EL1_LOW_MEMORY + i*THREAD_SIZE;
        }
    }
    return 0;
}

void free_kstack(unsigned long p){
    kstack_map[(p - EL1_LOW_MEMORY) / THREAD_SIZE] = 0;
}