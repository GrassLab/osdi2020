#include "../include/page.h"

struct page_manager PageManager;

struct page* translation(unsigned long kvirtual_address)
{
    return &PageManager.page_pool[kvirtual_address >> 12];
}

unsigned long page_alloc()
{
    return;
}

void page_free()
{
    return;
}