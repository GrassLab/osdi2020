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
            page_t* page = list_entry(tmp, page_t ,list);
            while(1){
                printf("start now buddy order: %d\n",page->order);
                printf("start now buddy used info: %d\n",page->used);
                printf("start now buddy addr: %x\n",page->phy_addr);
                printf("start now buddy index: %d\n",page->page_index);
                page_t* next_buddy_end_page = BUDDY_END(page, l);

                page_t* page_next = list_entry((&(BUDDY_END(page, l))->list)->next, page_t ,list);
                if((&(BUDDY_END(page, l))->list)->next == (&page_buddy[l])){
                    break;
                }
                page = page_next;
            }
        }else{
            printf("buddy system level empty value is: %d\n",l);
        }
    }
}

page_t* get_back_redundant_memory(page_t* alloc_page, int get_page_level, int req_page_level){
    for(int r = get_page_level-1 ; r >= req_page_level; r--){
        page_t* next_buddy_start_page = NEXT_BUDDY_START(alloc_page, r); 
        page_t* next_buddy_end_page = BUDDY_END(next_buddy_start_page, r);
        next_buddy_start_page->order = r;

        list_ptr_t* next_buddy_start =  &(next_buddy_start_page->list);    
		list_ptr_t* next_buddy_end = &(next_buddy_end_page->list);

        printf("****** cutoff memory from %d to %d(totally %d page) in order %d list\r\n",\
				next_buddy_start_page->page_index, next_buddy_end_page->page_index, \
				next_buddy_end_page->page_index - next_buddy_start_page->page_index+1, r);

        list_add_chain_tail(next_buddy_start,next_buddy_end,&page_buddy[r]);
    }
    return alloc_page;
}

page_t* get_pages_from_list(int order){
    page_t* alloc_page = NULL;
    int o = 0;
    for(o = order; o < MAX_ORDER; o++){
        if(!is_list_empty(&page_buddy[o])){
            alloc_page = list_entry(page_buddy[o].next, page_t, list);

            printf("request buddy order: %d\n",order);
            printf("want to allocate buddy order: %d\n",alloc_page->order);
            printf("want to allocate buddy used info: %d\n",alloc_page->used);
            printf("want to allocate buddy addr: %x\n",alloc_page->phy_addr);
            printf("want to allocate buddy index: %d\n",alloc_page->page_index);
            printf("want to allocate buddy total page number: %d\n",(1 << alloc_page->order));

            

            list_ptr_t* next_buddy =  (&(BUDDY_END(alloc_page, o)->list))->next;
            printf("-----------------\n");
            printf("request buddy order: %d\n",order);
            printf("want to allocate buddy order: %d\n",alloc_page->order);
            printf("want to allocate buddy used info: %d\n",alloc_page->used);
            printf("want to allocate buddy addr: %x\n",alloc_page->phy_addr);
            printf("want to allocate buddy index: %d\n",alloc_page->page_index);
            printf("want to allocate buddy total page number: %d\n",(1 << alloc_page->order));
            printf("page struct addr is : %x\n",&alloc_page);
            printf("-----------------\n");
            

            next_buddy->prev = (&page_buddy[o]);
            page_buddy[o].next = next_buddy;

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
                printf("addr is : %x\n",page_t_pool[p].phy_addr);
                printf("page struct addr is : %x\n",&page_t_pool[p]);
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

    get_pages_from_list(5);

    print_buddy_info();
    printf("------\n");

    get_pages_from_list(3);
    print_buddy_info();

    printf("------\n");

    get_pages_from_list(2);
    print_buddy_info();

    // printf("------\n");

    // get_pages_from_list(5);
    // print_buddy_info();
}