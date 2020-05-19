#include <stdint.h>

#include "io.h"
#include "page.h"
#include "task.h"

struct page_t pages[PAGE_MAX];
int page_now = 0x160;

struct page_t* page_alloc() {
    struct page_t* page = 0;
    for (int i = page_now; i < PAGE_MAX; i++) {
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
    struct page_t* user_page = page_alloc();
    task->user_page = user_page->id;
    struct page_t* pgd_page = page_alloc();
    struct page_t* pud_page = page_alloc();
    struct page_t* pmd_page = page_alloc();
    struct page_t* pte_page = page_alloc();
    uint64_t* pgd = pgd_page->content;
    uint64_t* pud = pud_page->content;
    uint64_t* pmd = pmd_page->content;
    uint64_t* pte = pte_page->content;
    *pgd = (uint64_t)pud | PD_TABLE;
    *pud = (uint64_t)pmd | PD_TABLE;
    *pmd = (uint64_t)pte | PD_TABLE;
    *pte = (uint64_t)user_page->content | PD_TABLE | PD_ACCESS;
    task->pgd = pgd;
    asm volatile("gg:");
    move_ttbr(task->pgd);
    asm volatile("gg2:");
}

void move_ttbr(uint64_t* pgd) { asm volatile("msr ttbr0_el1, x0"); }
