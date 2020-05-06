#include "mm.h"
#include "schedule.h"

struct stack_struct kstack_pool[TASK_POOL_SIZE];
struct stack_struct ustack_pool[TASK_POOL_SIZE];

char* get_avaliable_kstack() {
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        if (kstack_pool[i].avaliable) {
            kstack_pool[i].avaliable = 0;
            return kstack_pool[i].top;
        }
    }
    return 0;
}

char* get_avaliable_ustack() {
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        if (ustack_pool[i].avaliable) {
            ustack_pool[i].avaliable = 0;
            return ustack_pool[i].top;
        }
    }
    return 0;
}

void release_kstack(int task_id) {
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        if (kstack_pool[i].task_id == task_id) {
            kstack_pool[i].avaliable = 1;
            break;
        }
    }
}

void release_ustack(int task_id) {
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        if (ustack_pool[i].task_id == task_id) {
            ustack_pool[i].avaliable = 1;
            break;
        }
    }
}

void mm_init() {
    extern char _end;  // defined in linker
    char *kernel_end = ((char*) ((uint64_t) &_end & 0xfffffffffffffff0)) + PAGE_SIZE;  // sp need 16bit alignment, padding one page
    char *kstack0 = kernel_end + KSTK_SIZE;
    char *ustack0 = kernel_end + (TASK_POOL_SIZE * KSTK_SIZE) + USTK_SIZE;

    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        kstack_pool[i].top = kstack0 + i * KSTK_SIZE;
        kstack_pool[i].avaliable = 1;
        ustack_pool[i].top = ustack0 + i * USTK_SIZE;
        ustack_pool[i].avaliable = 1;
    }
}