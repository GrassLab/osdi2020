#include "type.h"
#include "task/taskStruct.h"
#include "memory/memManager.h"
#include "memory/mmConfig.h"

static struct page pages[1000];

uint64_t getFreePage()
{
    for (int i = 0; i < 1000; i++)
    {
        if (pages[i].used == false)
        {
            pages[i].used = true;
            uint64_t page = LOW_MEMORY + i * PAGE_SIZE;
            memzero(page + VA_START, PAGE_SIZE);
            return page;
        }
    }
    return 0;
}

uint64_t allocateKernelPage()
{
    uint64_t page = getFreePage();
    if (page == 0)
    {
        return 0;
    }
    return page + VA_START;
}

uint64_t allocateUserPage(struct task *task, uint64_t va) 
{
	uint64_t page = getFreePage();
	if (page == 0) 
    {
		return 0;
	}
	mapPage(task, va, page);
	return page + VA_START;
}

void freePage(uint64_t p)
{
    pages[(p - LOW_MEMORY) / PAGE_SIZE].used = false;
}

void mapTableEntry(uint64_t *pte, uint64_t va, uint64_t pa) {
	uint64_t index = va >> PAGE_SHIFT;
	index = index & (PTRS_PER_TABLE - 1);
	uint64_t entry = pa | MMU_PTE_FLAGS; 
	pte[index] = entry;
}

uint64_t mapTable(uint64_t *table, uint64_t shift, uint64_t va, int* new_table) {
	uint64_t index = va >> shift;
	index = index & (PTRS_PER_TABLE - 1);
	if (!table[index])
    {
		*new_table = 1;
		uint64_t next_level_table = getFreePage();
		uint64_t entry = next_level_table | PD_TABLE;
		table[index] = entry;
		return next_level_table;
	} else {
		*new_table = 0;
	}
	return table[index] & PAGE_MASK;
}

void mapPage(struct task *task, uint64_t va, uint64_t page){
	uint64_t pgd;
	if (!task->mm.pgd) {
		task->mm.pgd = getFreePage();
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = task->mm.pgd;
	}
	pgd = task->mm.pgd;
	int new_table;
	uint64_t pud = mapTable((uint64_t *)(pgd + VA_START), PGD_SHIFT, va, &new_table);
	if (new_table) {
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = pud;
	}
	uint64_t pmd = mapTable((uint64_t *)(pud + VA_START) , PUD_SHIFT, va, &new_table);
	if (new_table) {
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = pmd;
	}
	uint64_t pte = mapTable((uint64_t *)(pmd + VA_START), PMD_SHIFT, va, &new_table);
	if (new_table) {
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = pte;
	}
	mapTableEntry((uint64_t *)(pte + VA_START), va, page);
	struct user_page p = {page, va};
	task->mm.user_pages[task->mm.user_pages_count++] = p;
}