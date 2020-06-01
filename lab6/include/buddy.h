#ifndef _BUDDY_H
#define _BUDDY_H


#define MAX_ORDER 11

struct buddy{
    int page_frame_number;
    struct buddy* next;
};

struct buddy_head{
    int len;
    struct buddy* page;
};

struct buddy_head buddy_pool[MAX_ORDER];

void buddy_init();
int buddy_alloc(int size);


#endif