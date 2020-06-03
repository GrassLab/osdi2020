#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "type.h"

typedef struct allocator
{
    int size;
    uint64_t allocate_status;
    void * start_addr;

} allocator_t;

allocator_t * register_fixed_size_allocator ( int size );

#endif