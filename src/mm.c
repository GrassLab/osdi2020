#include "mm.h"

char *intr_stack;
char *kstack0;
char *ustack0;

void mm_init() {
    extern volatile unsigned char _end;  // defined in linker
    intr_stack = (char*)&_end + INTR_STK_SIZE;
    kstack0 = intr_stack + KSTK_SIZE;
    ustack0 = intr_stack + (TASK_POOL_SIZE * KSTK_SIZE) + USTK_SIZE;
}