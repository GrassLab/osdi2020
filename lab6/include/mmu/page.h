#ifndef __MMU_PAGE_H
#define __MMU_PAGE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct __Block {
    uint64_t lower;
    uint64_t upper;
} Block;

typedef struct __Page Page;

typedef struct __PageType {
    void (*init)(void);

    void (*pushFront)(Page **indirect_head, Block *block);
    void (*popFront)(Page **indirect_head);
    void (*insert)(Page **indirect_head, Page *page);
    void (*erase)(Page **indirect_next, Page *page);
    void (*showList)(Page *head);

    Page *(*allocate)(Page **indirect_head);
    void (*deallocate)(Page *page);

    bool (*inPage)(Page *page, uint64_t addr);

    Block *(*getBlock)(Page *page) __attribute__((const));
    Page *(*getNext)(Page *page) __attribute__((const));
    Page **(*getIndirectNext)(Page *page) __attribute__((const));
} PageType;

extern PageType gPage;

#endif
