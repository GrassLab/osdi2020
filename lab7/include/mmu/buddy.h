#ifndef __MMU_BUDDY_H
#define __MMU_BUDDY_H

#include <stddef.h>

// memory space for mantaining buddy system (size: 0x300000 = 512MB/4KB * sizeof(Page))
#define BUDDY_SYSTEM_BASE (0x100000 + VIRTUAL_ADDRESS_BASE)

typedef struct __Buddy Buddy;
typedef struct __Page Page;

// interface
typedef struct __BuddyType {
    void (*construct)(void);
    void (*show)(void);
    Page *(*allocate)(size_t block_size);
    void (*deallocate)(Page *page);
} BuddyType;

extern BuddyType gBuddy;

#endif
