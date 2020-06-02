#ifndef _BUDDY_H
#define _BUDDY_H

#define MAX_ORDER  9

struct buddyList
{
    struct page* head;
    size_t size;
};

uint64_t allocFreePage(uint32_t order);
void freePage(uint64_t addr);
void initBuddy();

#endif