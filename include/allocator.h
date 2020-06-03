#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "type.h"

typedef struct allocator
{
    int size;
    uint64_t allocate_status;
    intptr_t start_addr;

} allocator_t;

allocator_t * register_fixed_size_allocator ( int size );
void * alloc ( allocator_t * );
void free ( allocator_t *, void * );

#endif