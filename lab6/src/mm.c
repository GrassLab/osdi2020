#include "mm.h"
#include "arm/mmu.h"
#include "printf.h"
#define phy_to_vir(page) page + VA_START
#define pfn_to_phy(pfn) LOW_MEMORY + (pfn)*PAGE_SIZE
#define pfn_to_vir(pfn) LOW_MEMORY + (pfn)*PAGE_SIZE + VA_START
static int ind = 1;
static unsigned num_free_pages = PAGING_PAGES;
struct free_area buddy_entry[MAX_ORDER];
static int block_pool[MAX_PAGE]; // record the used of block
static struct block blocks[MAX_PAGE];

// return the index of free block address
int find_block(){
   for (int i = 0 ; i < MAX_PAGE ; i++) {
       if (block_pool[i] == 0) {
           block_pool[i] = 1;
           return i;
       }
   } 
   return -1;
}

void set_block(int index, int order, int page_nums, int pfn) {
    blocks[index].order = order;
    blocks[index].page_nums = page_nums;
    blocks[index].pfn = pfn;
    blocks[index].next = 0;
    blocks[index].record_index = index;
}

void insert_block(int index, int order, int page_nums, int pfn) {
    set_block(index, order , (1 << order), pfn);
    if (buddy_entry[order].head == 0) {
        buddy_entry[order].head = &blocks[index];
        buddy_entry[order].head->next = 0;
        buddy_entry[order].tail = buddy_entry[order].head;
        buddy_entry[order].nr_free += 1;
    }
    else {
        buddy_entry[order].tail->next = &blocks[index];
        buddy_entry[order].tail = buddy_entry[order].tail->next;
        buddy_entry[order].nr_free += 1;
    }
    return;
}

struct block *del_block(int order) {
    struct block *ptr = buddy_entry[order].head;
    if(ptr != 0) {
        block_pool[ptr->record_index] = 1;
        buddy_entry[order].head = ptr->next;
        if (buddy_entry[order].head == 0) {
            buddy_entry[order].tail = 0;
        }
        buddy_entry[order].nr_free -= 1;
    }
    
    return ptr;
}

void init_buddy_system() {
    // clear the block_pool
    memzero((unsigned long)block_pool, MAX_PAGE * sizeof(int));
    memzero((unsigned long)buddy_entry, MAX_ORDER * sizeof(struct free_area));
    int i;
    int page_num = 1;
    int pfn = 0;
    // i : order j : block
    for(i = 0 ; i < MAX_ORDER ; i++) {
        // create block
        int index = find_block();
        if (index == -1) {
            // wrong
            printf("Can't create block\r\n");
            return;
        }
        // set block
        insert_block(index, i, page_num, pfn);
        pfn += page_num;
        page_num = page_num * 2;
    }
}


void div_block(int alloc_order, int order) {
    //div alloc_order
    struct block *div_block = buddy_entry[alloc_order].head;
    for (int i = alloc_order - 1 ; i >= order ; i--) {
        if (i != order) {
            int block_index;
            block_index = find_block();
            insert_block(block_index, i, 1 << i, div_block->pfn + (1 << i));
        }
        else {
            int block_index;
            block_index = find_block();
            insert_block(block_index, i, 1 << i, div_block->pfn);
            block_index = find_block();
            insert_block(block_index, i, 1 << i, div_block->pfn + (1 << i));
        }
    }
    // free alloc_order block pop from head
    block_pool[div_block->record_index] = 0;
    buddy_entry[alloc_order].head = buddy_entry[alloc_order].head->next;
    if (buddy_entry[alloc_order].head == 0) {
        buddy_entry[alloc_order].tail = 0;
    }
    buddy_entry[alloc_order].nr_free -= 1;
}

// allocate pages from buddy system
struct block *alloc_block(int num_page) {
    struct block *ret_block;
    // num of pages
    int order = 0;
    while(num_page > (1 << order)) {
        order++;
        if(order == MAX_ORDER) {
            //wrong
            printf("There is no block for allocating \r\n");
            return 0;
        }
    }
    int alloc_order = order;
    // it is empty in that order
    if(buddy_entry[alloc_order].nr_free == 0) {
        while(buddy_entry[alloc_order].nr_free == 0) {
            alloc_order += 1;
        } 
        div_block(alloc_order, order);
    }
    ret_block = del_block(order);
    printf("allocate page pfn: %d\r\n", ret_block->pfn);
    print_buddy_entry(order);
    return ret_block;
}

int compute_buddy_block(int pfn, int order) {
    return pfn ^ (1 << order);
}

void print_buddy_entry(int order) {
    struct  block *ptr = buddy_entry[order].head;
    printf("--------\r\n");
    while(ptr != 0) {
        printf("%d ",ptr->pfn);
        ptr = ptr->next;
    }
    printf("\r\n--------\r\n");
    return;
}

struct block *find_buddy_pfn(int order, int buddy_pfn) {
    struct  block *ptr = buddy_entry[order].head;
    while(ptr != 0) {
        if (ptr->pfn == buddy_pfn) {
            return ptr;
        }
        ptr = ptr->next;
    }
    return ptr;
}

int del_buddy_block(int order, int buddy_pfn) {
    struct  block *ptr = buddy_entry[order].head;
    struct  block *prev = 0;
    while(ptr != 0) {
        if (ptr->pfn == buddy_pfn) {
            block_pool[ptr->record_index] = 0;
            // delete buddy_pfn
            if (prev == 0) {
                buddy_entry[order].head = ptr->next;
                if (buddy_entry[order].head == 0) buddy_entry[order].tail = 0;
            }
            else {
                prev->next = ptr->next;
                if (ptr->next == 0)  buddy_entry[order].tail = buddy_entry[order].head;
            }
            buddy_entry[order].nr_free -= 1;
            return 1;
        }
        prev = ptr;
        ptr = ptr->next;
    }
    return 0;
}

// free block and alloc and combine
void put_back_block(struct block *free_block) {
    // block's order free_block->record_index
    int block_order = free_block->order;
    int pfn = free_block->pfn;
    printf("put back page pfn: %d order: %d\r\n", pfn, block_order);
    // combine
    while (block_order < MAX_ORDER) {
        // compute the buddy block
        int buddy_pfn = compute_buddy_block(pfn, block_order);
        printf("buddy_block: %d order: %d\r\n", buddy_pfn, block_order);
        struct block *tmp = find_buddy_pfn(block_order, buddy_pfn);
        if (tmp == 0) {
            printf("buddy_block not find\r\n");
            //TODO can't find buddy block 
            int block_index;
            block_index = find_block();
            printf("insert_block: %d order: %d\r\n", pfn, block_order);
            insert_block(block_index, block_order, (1 << block_order), pfn);
            print_buddy_entry(block_order);
            break;
        }
        // free tmp block
        del_buddy_block(block_order, buddy_pfn);
        printf("del buddy block: %d in order: %d\r\n", buddy_pfn, block_order);
        print_buddy_entry(block_order);
        pfn = buddy_pfn < pfn ? buddy_pfn : pfn;
        block_order += 1;
    }
    printf("\r\n");
    // free the block_pool
    block_pool[free_block->record_index] = 0;
}

struct block *get_free_page()
{
	struct block *alloc = alloc_block(1);
    memzero(pfn_to_vir(alloc->pfn), PAGE_SIZE);
	return alloc;
}

struct block *allocate_kernel_page() 
{
    struct block *alloc = get_free_page(); //TODO record
    num_free_pages -= 1;
    //printf("allocate kernel page pfn: %d\r\n", alloc->pfn);
    return alloc;
}

unsigned long allocate_user_page(struct task_struct *task, unsigned long va, unsigned long prot) {
	struct block *alloc = get_free_page();
	map_page(task, va, alloc, prot);
    //printf("allocate user page pfn: %d\r\n", alloc->pfn);
    num_free_pages -= 1;
	return pfn_to_vir(alloc->pfn);
}

unsigned long map_table(unsigned long *table, unsigned long shift, unsigned long va, struct task_struct *task) {
	unsigned long index = va >> shift; // find the index of page table
	index = index & (PTRS_PER_TABLE - 1); // mask the attributes
	if (!table[index]){ 
        struct block *alloc_tmp = allocate_kernel_page();
        task->mm.kernel_pages[task->mm.kernel_pages_count++] = alloc_tmp;
		unsigned long next_level_table = pfn_to_phy(alloc_tmp->pfn);
		unsigned long entry = next_level_table | MM_TYPE_PAGE_TABLE; //entry is physical address | MM_TYPE 
		table[index] = entry;
		return next_level_table;
	}
	return table[index] & PAGE_MASK;
}

void map_page(struct task_struct *task, unsigned long va, struct block *alloc, unsigned long prot){
    unsigned long page = pfn_to_phy(alloc->pfn);
	unsigned long pgd;
	if (!task->mm.pgd) {
        struct block *alloc_t = allocate_kernel_page();
		task->mm.pgd = pfn_to_phy(alloc_t->pfn); // allocate a page and return physical address
		task->mm.kernel_pages[task->mm.kernel_pages_count++] = alloc_t;
	}
	pgd = task->mm.pgd;
	unsigned long pud = map_table((unsigned long *)(phy_to_vir(pgd)), PGD_SHIFT, va, task);
	unsigned long pmd = map_table((unsigned long *)(phy_to_vir(pud)), PUD_SHIFT, va, task);
	unsigned long pte = map_table((unsigned long *)(phy_to_vir(pmd)), PMD_SHIFT, va, task);
	map_table_entry((unsigned long *)(phy_to_vir(pte)), va, page, prot);
    task->mm.user_va[task->mm.user_pages_count] = va;
	task->mm.user_pages[task->mm.user_pages_count++] = alloc;
}


void map_table_entry(unsigned long *pte, unsigned long va, unsigned long pa, unsigned long flag) {
	unsigned long index = va >> PAGE_SHIFT;
	index = index & (PTRS_PER_TABLE - 1);
	unsigned long entry = pa | flag; // put protection flag
	pte[index] = entry;
}

void free_user_page(struct task_struct *task) {
	for (int i = 0 ; i < task->mm.user_pages_count ; i++) {
		put_back_block(task->mm.user_pages[i]);
		num_free_pages += 1;
	}
    task->mm.user_pages_count = 0;
	return;
}

void free_kernel_page(struct task_struct *task) {
	for (int i = 0 ; i < task->mm.kernel_pages_count ; i++) {
        put_back_block(task->mm.kernel_pages[i]);
		num_free_pages += 1;
	}
    task->mm.user_pages_count = 0;
	return;
}

int copy_virt_memory(struct task_struct *dst) {
	struct task_struct* src = current;
	for (int i = 0; i < src->mm.user_pages_count; i++) {
        unsigned long virtual_addr = src->mm.user_va[i];
		unsigned long kernel_va = allocate_user_page(dst, virtual_addr, MMU_PTE_FLAGS);
		if( kernel_va == 0) {
			return -1;
		}
		memcpy(virtual_addr, kernel_va, PAGE_SIZE);
    }
	return 0;
}

int do_mem_abort(unsigned long addr, unsigned long esr) {
	unsigned long dfs = (esr & 0b111111);
	if ((dfs & 0b111100) == 0b100) {
		ind++;
		if (ind > 2){
			unsigned long val;
			printf("esr: 0x%x\r\n", dfs);
			asm volatile ("mrs %0, far_el1" : "=r" (val));
			printf("page fault address: 0x%x\r\n", val);
			exit_process();
			return -1;
		}
        else {
            allocate_user_page(current, addr & PAGE_MASK, MMU_PTE_FLAGS);
        }
		return 0;
	}
	else {
		printf("page fault\r\n");
		exit_process();
	}
	return -1;
}

int get_remain_num() {
	return num_free_pages;
}

void free_zombie_task() {
	for (int i = 0 ; i < nr_tasks ; i++) {
		if (task[i]->state == TASK_ZOMBIE) {
			free_kernel_page((unsigned long)task[i]);
			task[i]->state = TASK_FREE;
			num_free_pages += 1;
		}
	}
} 
