#pragma once

#include "list.h"

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

int cal_order(int size);
void buddy_init(int num_pages);
void buddy_show();
int buddy_alloc(int size);
int pfn2phy(int page_frame_number);
void check_merge();
void buddy_free(int page_frame_number, int page_frame_size);