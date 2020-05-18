#ifndef __MMU_VMA_H
#define __MMU_VMA_H

#include <stdint.h>

typedef struct __Page {
    uint64_t in_use;
    struct __Page *next;
} Page;

void initPageFrames(void);

#endif
