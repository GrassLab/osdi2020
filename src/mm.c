#include "mm.h"
#include "schedule.h"

char* kstack_pool[TASK_POOL_SIZE];
char* ustack_pool[TASK_POOL_SIZE];
int kstack_avaliable[TASK_POOL_SIZE];
int ustack_avaliable[TASK_POOL_SIZE];

char* get_avaliable_kstack() {
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        if (kstack_avaliable[i]) {
            kstack_avaliable[i] = 0;
            return kstack_pool[i];
        }
    }
    return 0;
}

char* get_avaliable_ustack() {
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        if (ustack_avaliable[i]) {
            ustack_avaliable[i] = 0;
            return ustack_pool[i];
        }
    }
    return 0;
}

void mm_init() {
    extern char _end;  // defined in linker
    char *kernel_end = ((char*) ((uint64_t) &_end & 0xfffffffffffffff0)) + PAGE_SIZE;  // sp need 16bit alignment, padding one page
    char *kstack0 = kernel_end + KSTK_SIZE;
    char *ustack0 = kernel_end + (TASK_POOL_SIZE * KSTK_SIZE) + USTK_SIZE;

    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        kstack_pool[i] = kstack0 + i * KSTK_SIZE;
        kstack_avaliable[i] = 1;
        ustack_pool[i] = ustack0 + i * USTK_SIZE;
        ustack_avaliable[i] = 1;
    }
}