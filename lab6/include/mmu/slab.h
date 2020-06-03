#ifndef __MMU_SLAB_H
#define __MMU_SLAB_H

#include <stddef.h>
#include <stdint.h>

typedef struct __SlabType {
    void (*init)(void);

    uint64_t (*regist)(size_t object_size);
    void *(*allocate)(uint64_t token);
    void (*deallocate)(void *ptr);
} SlabType;

extern SlabType gSlab;

size_t alignSize(size_t size);

#endif
