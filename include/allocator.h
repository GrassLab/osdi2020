#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "type.h"

typedef struct fixed_allocator
{
    int size;
    uint64_t allocate_status;
    intptr_t start_addr;

} fixed_allocator_t;

typedef struct dynamic_allocator_page_alloc_record
{
    intptr_t pade_addr;

    struct dynamic_allocator_page_alloc_record * next;
} page_alloc_record_t;

typedef struct dynamic_allocator
{
    fixed_allocator_t * byte_64_fixed;
    fixed_allocator_t * byte_128_fixed;
    fixed_allocator_t * byte_256_fixed;

    page_alloc_record_t * page_alloc_list;

} dynamic_allocator_t;

fixed_allocator_t * register_fixed_size_allocator ( int size );
dynamic_allocator_t * register_varied_size_allocator ( );
void * fixed_alloc ( fixed_allocator_t * );
void fixed_free ( fixed_allocator_t *, void * );

#endif