#ifndef ALLOCATOR_H
#define ALLOCATOR_H


#include "memory.h"

#define SLUB_NUMBER_MAX 4096
#define ALLOCATOR_NUMBER_MAX 128
#define ALLOCATOR_PAGE_NUMBER_MAX 128

#define SLUB_NOT_USED 0
#define SLUB_USED 1

typedef struct slub {
    int used;
    int objectSize;
    unsigned long address;
    struct slub *prev;
    struct slub *next;
} Slub;

typedef struct allocator {
    Slub *slub;
    Page *pages[ALLOCATOR_PAGE_NUMBER_MAX];
    int pageCount;
    int objectSize;
} Allocator;

Slub slubs[SLUB_NUMBER_MAX];
Allocator allocators[ALLOCATOR_NUMBER_MAX];

void allocator_init();
void varied_size_init();
void allocator_register(int size);
Allocator* find_allocator(int size);
Slub* allocate_object(int size);
void free_object(Slub *slub);
unsigned long allocate_memory(int size);
void free_memory();

#endif
