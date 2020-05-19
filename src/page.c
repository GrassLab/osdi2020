#include <stdint.h>

#include "io.h"
#include "page.h"
#include "task.h"

struct page_t pages[PAGE_MAX];
int page_now = 0;

struct page_t* page_alloc() {
    struct page_t* page = 0;
    for (int i = 0; i < PAGE_MAX; i++) {
        if (pages[i].status == FREE) {
            page = &pages[i];
            page->id = i;
            page->content = (uint64_t*)((uint64_t)i << 12);
            break;
        }
    }
    page->status = ALLOCATED;
    return page;
}

void page_free(struct page_t* page) {
    if (page->status == FREE) return;
    uint64_t address_base = page->id << 12;
    for (int i = 0; i < 1 << 12; i++) {
        volatile uint8_t* address = (volatile uint8_t*)(address_base + i);
        *address = 0;
    }
}

void page_mapping(struct task_t* task) {
    struct page_t* pgd_page = page_alloc();
    struct page_t* pud_page = page_alloc();
    struct page_t* pmd_page = page_alloc();
    struct page_t* pte_page = page_alloc();
    task->pgd = (uint64_t)(pgd_page->id) << 12;
    uint64_t* pud = pgd_page->content;
    uint64_t* pmd = pgd_page->content;
    uint64_t* pte = pgd_page->content;
}
