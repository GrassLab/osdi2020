#include "mm.h"
#include "schedule.h"

char* kstack_pool[TASK_POOL_SIZE];

void mm_init() {
    extern char _end;  // defined in linker
    char *kernel_end = ((char*) ((uint64_t) &_end & 0xfffffffffffffff0)) + PAGE_SIZE;  // sp need 16bit alignment, padding one page
    char *kstack0 = kernel_end + KSTK_SIZE;
    // char *ustack0 = kernel_end + (TASK_POOL_SIZE * KSTK_SIZE) + USTK_SIZE;

    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        kstack_pool[i] = kstack0 + i * KSTK_SIZE;
    }
}