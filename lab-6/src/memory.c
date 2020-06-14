#include "memory.h"
#include "printf.h"
#include "list.h"
#include "uart.h"
#define NULL 0

void page_sys_init() {
	for(int i = 0; i <= BUDDY_MAX_ORDER; i++) {
		LIST_INIT(&pageBuddy[i]);
	}
	for(int p = 0; p < TOTAL_PAGE_NUMBER; p++) {
		pagePool[p].index = p;
		pagePool[p].used = PAGE_NOT_USED;
		pagePool[p].order = -1;
		pagePool[p].physicalAddr = LOW_PAGE_POOL_MEMORY + p * PAGE_SIZE;
		LIST_INIT(&(pagePool[p].list));
        if (p == 0) pagePool[p].order = BUDDY_MAX_ORDER;
        list_add_tail(&(pagePool[p].list), &pageBuddy[BUDDY_MAX_ORDER]);
	}
}

void print_buddy() {
    for(int order = 0; order <= BUDDY_MAX_ORDER; order++) {
		if(!is_list_empty(&pageBuddy[order])){
            List *tmp = pageBuddy[order].next;
            Page *page = list_entry(tmp, Page ,list);
            while(1) {
                // printf("[buddy info] buddy order: %d\n", page->order);
                // printf("[buddy info] buddy used: %d\n", page->used);
                // printf("[buddy info] buddy addr: %x\n", page->physicalAddr);
                // printf("[buddy info] buddy index: %d\n\n", page->index);
                Page *page_next = list_entry((&(BUDDY_END(page, order))->list)->next, Page ,list);
				if((&(BUDDY_END(page, order))->list)->next == (&pageBuddy[order])) {
                    break;
                }
                page = page_next;
            }
        } else {
            // printf("[buddy info] order %d list is empty\n", order);
        }
	}
}

Page* get_page(int order) {
	Page *alloc_page = NULL;
	int o;
	for(o = order; o <= BUDDY_MAX_ORDER; o++){
        if (!is_list_empty(&pageBuddy[o])) {
            alloc_page = list_entry(pageBuddy[o].next, Page, list);
            List *next_buddy =  (&(BUDDY_END(alloc_page, o)->list))->next;
            // printf("[request info] request order: %d\n", order);
            // printf("[request info] plan to allocate order: %d\n",alloc_page->order);
            // printf("[request info] plan to allocate used info: %d\n",alloc_page->used);
            // printf("[request info] plan to allocate addr: 0x%x\n",alloc_page->physicalAddr);
            // printf("[request info] plan to allocate index: %d\n",alloc_page->index);
            // printf("[request info] plan to allocate page numbers: %d\n\n",(1 << alloc_page->order));
            next_buddy->prev = &pageBuddy[o];
            pageBuddy[o].next = next_buddy;
            break;
        }
    }
	if (o == BUDDY_MAX_ORDER + 1) {
        // printf("[buddy error] do not enough memory to allocate!\n");
        return NULL;
    }

    // alloc_page->order = order;
    if (o != order) {
        alloc_page = release_redundant_memory(alloc_page, o, order);
    }
    alloc_page->used = PAGE_USED;
    // printf("[allocate] index: %d - %d\n", alloc_page->index, alloc_page->index + (1 << order) - 1);
    // printf("[allocate] used: %d \n", alloc_page->used);
    // printf("[allocate] order: %d \n", alloc_page->order);
    return alloc_page;
}

Page* release_redundant_memory(Page *alloc_page, int get_page_order, int req_page_order) {
    for (int r = get_page_order - 1 ; r >= req_page_order; r--){
        Page* next_buddy_start_page = NEXT_BUDDY_START(alloc_page, r); 
        Page* next_buddy_end_page = BUDDY_END(next_buddy_start_page, r);
        next_buddy_start_page->order = r;

        List* next_buddy_start =  &(next_buddy_start_page->list);    
		List* next_buddy_end = &(next_buddy_end_page->list);

        // printf("[release redundant] from index %d to %d (totally %d page) in order %d list\r\n",\
		// 		next_buddy_start_page->index, next_buddy_end_page->index, \
		// 		next_buddy_end_page->index - next_buddy_start_page->index+1, r);

        list_add_chain_tail(next_buddy_start, next_buddy_end, &pageBuddy[r]);
    }
    alloc_page->order = req_page_order;
    return alloc_page;
}


void free_page(Page *page) {
    int order = page->order; 
    for (int i = page->index; i < (page->index + (1 << order)); i++) {
		pagePool[i].used = PAGE_NOT_USED;
	}
    // buddy_page_index: plan to be merged page
    int buddy_page_index = page->index ^ (1 << order);
    Page *head_page = page;
    Page *end_page = NULL;
    Page *buddy_page = NULL;

    for (;order < BUDDY_MAX_ORDER; order++) {
        buddy_page_index = page->index ^ (1 << order);
        buddy_page = &pagePool[buddy_page_index];
        // printf("[merge buddy] check page order: %d\n", page->order);
        // printf("[merge buddy] check page index: %d - %d\n", page->index, page->index + (1<< order) - 1);
        // printf("[merge buddy] check buddy index: %d - %d\n", buddy_page_index, buddy_page_index+ (1<< order) - 1);
        // printf("[merge buddy] check buddy used: %d\n", pagePool[buddy_page_index].used);
        if (buddy_page->used == PAGE_NOT_USED && buddy_page->order == page->order) {
            list_remove_chain(&(buddy_page->list), &(BUDDY_END(buddy_page, order)->list));
            
            head_page = (buddy_page->index < page->index) ? buddy_page : page; // min
            end_page = (buddy_page->index < page->index) ? page : buddy_page;  // max

            buddy_page->order = -1;
            head_page->order = order + 1;

            // printf("[merge buddy] %d to %d\r\n", head_page->index, BUDDY_END(end_page, order)->index);
            // printf("[merge buddy] merged page order is %d \r\n", head_page->order);

            // link between the same order buddy, tail of ahead page link head of behind page
            BUDDY_END(head_page, order)->list.next = &(end_page->list);
            end_page->list.prev = &(BUDDY_END(head_page, order)->list);

            page = head_page;
            if (order + 1 == BUDDY_MAX_ORDER + 1) break;
        } else {
            break;
        }
    } 
    // printf("[merge buddy] finished merged order %d \n", order);
    // merge into buddy list
    list_add_chain(&(page->list), &(BUDDY_END(page, order)->list), &pageBuddy[order]);
}