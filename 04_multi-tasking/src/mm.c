#include "mm.h"

unsigned char page_map[NUM_PAGES] = {0,};

/* simple allocate a continuous memory */
unsigned long allocate_a_page() {
    for (int i = 0; i < NUM_PAGES; ++ i) {
        if (page_map[i] == 0) {
            page_map[i] = 1;
            return LOW_MEMORY + i * PAGE_SIZE;
        }
    }
    // 0 means allocate failed
    return 0;
}

void free_a_page(unsigned long ptr) {
    page_map[(ptr - LOW_MEMORY) / PAGE_SIZE] = 0;
}
