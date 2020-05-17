#include "mm.h"

unsigned short memory_map[NUM_PAGES] = {0,};

/* simple allocate a continuous memory */
unsigned long allocate_task_struct() {
    for (int i = 0; i < NUM_PAGES; ++ i) {
        if (memory_map[i] == 0) {
            memory_map[i] = 1;
            return LOW_MEMORY + i * PAGE_SIZE;
        }
    }
    // 0 means allocate failed
    return 0;
}

void free_task_struct(unsigned long ptr) {
    memory_map[(ptr - LOW_MEMORY) / PAGE_SIZE] = 0;
}
