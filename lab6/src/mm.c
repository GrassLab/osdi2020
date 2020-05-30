#include "mm.h"
#include "config.h"

void init_buddy_sys(){
	for(int i=0;i<MAX_ORDER;i++){
		INIT_LIST_POINTER(&page_buddy[i]);
	}
}

void print_buddy_info(){
    for(int l = 0; l < MAX_ORDER; l++){
        if(!is_list_empty(&page_buddy[l])){
            list_ptr_t *tmp = page_buddy[l].next;
            do{
                page_t* page = list_entry(tmp, page_t ,list);
                if(page->order > 0){
                    printf("now buddy order: %d\n",page->order);
                    printf("now buddy used info: %d\n",page->used);
                    printf("now buddy addr: %d\n",page->phy_addr);
                }
                tmp = tmp->next;
            }while(tmp != page_buddy[l].next);
        }else{
            printf("buddy system level empty value is: %d\n",l);
        }
    }
}

page_t* get_back_redundant_memory(page_t* alloc_page, int get_page_level, int req_page_level){
    for(; req_page_level < get_page_level; get_page_level--){

    }
    return alloc_page;
}

page_t* get_pages_from_list(int order){
    page_t* alloc_page = NULL;
    int o = 0;
    for(o = order; o < MAX_ORDER; o++){
        if(!is_list_empty(&page_buddy[o])){
            alloc_page = list_entry(page_buddy[o].next, page_t, list);

            printf("want to allocate buddy order: %d\n",alloc_page->order);
            printf("want to allocate buddy used info: %d\n",alloc_page->used);
            printf("want to allocate buddy addr: %d\n",alloc_page->phy_addr);
            
            list_ptr_t* next_buddy = (&(NEXT_BUDDY_START(alloc_page, o))->list);
            
            next_buddy->prev = (&page_buddy[o]);
            page_buddy[o].next = next_buddy;

            alloc_page = list_entry(next_buddy, page_t, list);

            break;
        }
    }

    if(o == MAX_ORDER){
        printf("cannot have enough memory to allocate!\n");
        return NULL;
    }
    if(o != order){
        alloc_page = get_back_redundant_memory(alloc_page, o, order);
    }
    return alloc_page;
}

void allocate_mem_from_buddy(int order){
    
}

void init_page_sys(){
    printf("total page number is: %d\n", TOTAL_PAGE_NUMBER);
    printf("total page number is: %x\n", HIGH_PAGE_POOL_MEMORY);
    printf("total page number is: %d\n", MAX_BUDDY_PAGE_NUMBER);
    init_buddy_sys();
    
    int remain_counter = 0;
    for(int p = 0; p < TOTAL_PAGE_NUMBER; p++){
        page_t_pool[p].used = NOT_USED;
        page_t_pool[p].order = 0;
        page_t_pool[p].page_index = p;
        page_t_pool[p].phy_addr = LOW_PAGE_POOL_MEMORY + p * PAGE_SIZE;
        INIT_LIST_POINTER(&(page_t_pool[p].list));

        if(remain_counter == 0){
            if((p + MAX_BUDDY_PAGE_NUMBER) <= TOTAL_PAGE_NUMBER){
                remain_counter = MAX_BUDDY_PAGE_NUMBER-1;
                printf("allocate max memory 1MB\n");
                printf("head value is:%d\n", p);
                printf("end value is:%d\n", (p+MAX_BUDDY_PAGE_NUMBER-1));
                printf("remain_counter is : %d\n",remain_counter);
                list_add_tail(&(page_t_pool[p].list),&page_buddy[MAX_ORDER-1]);
                page_t_pool[p].order = MAX_ORDER-1;
            }
            else{
                list_add_tail(&(page_t_pool[p].list),&page_buddy[0]);
                page_t_pool[p].order = 0;
            }
        }
        else{
            page_t_pool[p].order = -1;
            list_add_tail(&(page_t_pool[p].list),&page_buddy[MAX_ORDER-1]);
            remain_counter--;
        }
    }

    print_buddy_info();
    printf("------\n");

    get_pages_from_list(8);

    print_buddy_info();
}