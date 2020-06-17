#include "mm.h"
#include "config.h"

static int free_page_number = TOTAL_PAGE_NUMBER;

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
                delay(1000000);
                printf("start now buddy order: %d\n",page->order);
                // printf("start now buddy used info: %d\n",page->used);
                // printf("start now buddy addr: %x\n",page->phy_addr);
                // printf("start now buddy index: %d\n",page->page_index);
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

        printf("****** cutoff memory from %d to %d (totally %d page) in order %d list *******\r\n",\
				next_buddy_start_page->page_index, next_buddy_end_page->page_index, \
				next_buddy_end_page->page_index - next_buddy_start_page->page_index+1, r);

        list_add_chain_tail(next_buddy_start,next_buddy_end,&page_buddy[r]);
    }
    alloc_page->order = req_page_level;
    return alloc_page;
}

page_t* get_pages_from_list(int order){
    page_t* alloc_page = NULL;
    int o = 0;
    for(o = order; o < MAX_ORDER; o++){
        if(!is_list_empty(&page_buddy[o])){
            alloc_page = list_entry(page_buddy[o].next, page_t, list);

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

    alloc_page->order = order;
    if(o != order){
        alloc_page = get_back_redundant_memory(alloc_page, o, order);
    }
    alloc_page->used = USED;
    printf("allocate mem index is %d - %d\n", alloc_page->page_index, alloc_page->page_index + (1 << order) - 1);
    printf("allocate mem used number is %d \n", alloc_page->used);
    printf("allocate mem order is %d \n", alloc_page->order);
    return alloc_page;
}

unsigned long physical_to_pfn(unsigned long physical_addr){
	return (physical_addr - LOW_PAGE_POOL_MEMORY) >> 12;	
}

void give_back_pages(page_t* page , int order){
    int buddy_page_index = page->page_index ^ (1 << order);
    int is_buddy_bigger_than_self = 0;
    page_t* head_page = page;
    page_t* end_page = NULL;
    page_t* buddy_page = NULL;

    for(; order < MAX_ORDER - 1; order++){
        buddy_page_index = page->page_index ^ (1 << order);
        buddy_page = &page_t_pool[buddy_page_index];
        printf("receive number is %d - %d\n", page->page_index, page->page_index + (1<< order));
        printf("now buddy number is %d - %d\n", buddy_page_index, buddy_page_index+ (1<< order));
        printf("now buddy number is used %d\n", page_t_pool[buddy_page_index].used);
        if(buddy_page->used == NOT_USED && buddy_page->order == page->order){
            list_remove_chain(&(buddy_page->list), &(BUDDY_END(buddy_page, order)->list));
            head_page = (buddy_page->page_index < page->page_index)?buddy_page:page;
            // printf(">>> head page index is %d \r\n", page->page_index);
            // printf(">>> end page index is %d \r\n", buddy_page->page_index);
            end_page = (buddy_page->page_index < page->page_index)?page:buddy_page;
            // printf(">>> head page index is %d \r\n", page->page_index);
            // printf(">>> end page index is %d \r\n", buddy_page->page_index);

            buddy_page->order = -1;
            head_page->order = -1;
            
            head_page->order = order + 1;
            printf(">>> merge %d to %d\r\n", head_page->page_index, BUDDY_END(end_page, order)->page_index);
            printf(">>> head page order is %d \r\n", head_page->order);


            //link between the same order buddy, tail of ahead page link head of behind page
            BUDDY_END(head_page, order)->list.next = &(end_page->list);
            end_page->list.prev = &(BUDDY_END(head_page, order)->list);

            page = head_page;
            if(order + 1 == MAX_ORDER){
                break;
            }
        }
        else{
            break;
        }
    } 
    printf("---------------merge into order%d \n", order);
    //merge into link list
    list_add_chain(&(page->list), &(BUDDY_END(page, order)->list), &page_buddy[order]);
}

void free_page(unsigned long physical_addr){ 
	unsigned long page_frame_number = physical_to_pfn(physical_addr);
	
    printf("*************************************************\n");
	for(unsigned int i = page_frame_number; i < (page_frame_number + (1 << page_t_pool[page_frame_number].order)); i++){
		page_t_pool[i].used = NOT_USED;
        // printf("order is :%d\n", page_t_pool[i].order);
		free_page_number++;
	}
	
	give_back_pages(&page_t_pool[page_frame_number], page_t_pool[page_frame_number].order);	
}

page_t *alloc_pages(int order){
	page_t *page = get_pages_from_list(order);
	if(page == (page_t*)NULL){
		return (page_t*)NULL;
    }
	
	for(int i = 0; i < (1<<order); i++){
		(page + i)->used = USED;
		memzero(page[i].phy_addr, PAGE_SIZE);
		free_page_number--;
	}
	return page;
}

int slub_size_to_index(int size){
    int i = 0;
    int s = size;
    while (s >>= 1){
        i++;
    }
    i = (size == (1 << (i)))?(i):i+1;
    return i - 3;
}


unsigned long allocate_memory(int size){
    if(size > 4096){
        int page_num = size >> 12;
        page_t* p = get_pages_from_list(page_num);
        return p->phy_addr;
    }
    return give_slab(size);
}

void receive_memory(unsigned long physical_addr){
    unsigned long page_frame_number = physical_to_pfn(physical_addr);
    if(page_t_pool[page_frame_number].slub_index != -1){
        receive_slub(physical_addr);
    }
    else{
        free_page(physical_addr);
    }
}

void print_slub_info(){
    for(int i = 0 ; i < SLUB_NUMBER; i++){
        printf("--------------------\n");
        printf("now slub size is: %d\n", SLUB_INDEX_TO_SIZE(i));
        slub_t* init_ptr = kmem_cache_arr[i].cache_cpu.free_list;
        while(init_ptr != NULL){
            printf("now addr is: %x\n", init_ptr);
            init_ptr = init_ptr->next;
        }
    }
}

void split_page_to_slub(page_t* page, int slub_index){
    int size = SLUB_INDEX_TO_SIZE(slub_index);
    unsigned long start_addr = page->phy_addr;
    page->slub_next = (slub_t *)(start_addr);
    page->slub_index = slub_index;
    kmem_cache_arr[slub_index].cache_cpu.free_list = page->slub_next;

    slub_t * tmp;
    int slub_num = PAGE_SIZE/size;
    page->slub_num = slub_num;
    for(int i = 0; i < slub_num -1; i++){
        tmp = (slub_t *)(start_addr + i*size);
        printf("________________________________\n");
        printf("split page addr is:%x\n", tmp);
        tmp->next = (slub_t *)(start_addr + (i+1)*size);
    }
    tmp = (slub_t *)(start_addr + (slub_num-1)*size);
    printf("________________________________\n");
    printf("split page addr is:%x\n", tmp);
    tmp->next = NULL;
}

void init_kmalloc_caches(){
    for(int s = 0; s < SLUB_NUMBER; s++){
        kmem_cache_arr[s].cache_cpu.free_list = NULL;
        kmem_cache_arr[s].cache_cpu.free_list->next = NULL;
        kmem_cache_arr[s].cache_cpu.page = NULL;
        INIT_LIST_POINTER(&kmem_cache_arr[s].cache_cpu.partial);
    }
}

unsigned long give_one_slub(int slub_index){
    slub_t* slub = kmem_cache_arr[slub_index].cache_cpu.free_list;
    printf("######################\n");
    // printf("allocate slub addr is: %x\n", slub->phy_addr);
    printf("slub want to allocate slub addr is: %x\n", slub);
    kmem_cache_arr[slub_index].cache_cpu.page->slub_next = slub->next;
    kmem_cache_arr[slub_index].cache_cpu.free_list = slub->next;
    
    kmem_cache_arr[slub_index].cache_cpu.page->slub_num--;
    if(kmem_cache_arr[slub_index].cache_cpu.page->slub_num == 0){
        printf("&&&&&&&&&&&&&&&&&&&&&&& add to partial page number is %d\n",kmem_cache_arr[slub_index].cache_cpu.page->page_index);
        list_add_tail(&(kmem_cache_arr[slub_index].cache_cpu.page->list),&kmem_cache_arr[slub_index].cache_cpu.partial);
    }
    return slub;
}

page_t* find_partial_free_slub(int slub_index){
    if(!is_list_empty(&kmem_cache_arr[slub_index].cache_cpu.partial)){
        printf("xxxxxxxxxxxxxxxxpartial list not empty\n");
        page_t* page = NULL;
        list_ptr_t* tmp = kmem_cache_arr[slub_index].cache_cpu.partial.next;
        do{
            delay(1000000);
            page = list_entry(tmp, page_t ,list);
            printf("partial page number is %d\n", page->page_index);
            printf("partial page order is %d\n", page->order);
            printf("partial page slub index is %d\n", page->slub_index);
            printf("partial page slub num is %d\n", page->slub_num);
            printf("-----\n");
            if(page->slub_num >= 1){
                printf("partial has a free slub!!!!!!!!!\n");
                return page;
            }
            tmp = tmp->next;
        // because we are circular link list ,so head->next-> .... -> = head, it should point to head again
        }while(page->list.next != &kmem_cache_arr[slub_index].cache_cpu.partial);

    }
    printf("xxxxxxxxxxxxxxxxpartial has no free slub\n");
    return (page_t*)NULL;
}

unsigned long give_slab(int size){
    int slub_index = slub_size_to_index(size);
    printf("slub index is %d\n",slub_index);
    if(kmem_cache_arr[slub_index].cache_cpu.free_list == NULL){
        page_t* parital_free_page = find_partial_free_slub(slub_index);
        // if partial has free page we can use, then set it as free list and page pointer to it,
        // and remove it from partial chain
        if(parital_free_page != NULL){
            kmem_cache_arr[slub_index].cache_cpu.free_list = parital_free_page->slub_next;
            kmem_cache_arr[slub_index].cache_cpu.page = parital_free_page;
            list_remove_chain(&(kmem_cache_arr[slub_index].cache_cpu.page->list), &(kmem_cache_arr[slub_index].cache_cpu.page->list));
        }
        // request a new page from buddy
        else{
            printf("%d slub is empty!\n", SLUB_INDEX_TO_SIZE(slub_index));
            page_t* p = get_pages_from_list(0);
            kmem_cache_arr[slub_index].cache_cpu.page = p;
            print_buddy_info();
            split_page_to_slub(p, slub_index);
        }
    }
    return give_one_slub(slub_index);
}

void init_page(int page_index){
    page_t_pool[page_index].used = NOT_USED;
    // page_t_pool[page_index].order = 0;
    page_t_pool[page_index].page_index = page_index;
    page_t_pool[page_index].phy_addr = LOW_PAGE_POOL_MEMORY + page_index * PAGE_SIZE;
    page_t_pool[page_index].slub_next = NULL;
    page_t_pool[page_index].slub_num = -1;
    page_t_pool[page_index].slub_index = -1;
}

void receive_slub(unsigned long physical_addr){
    unsigned long page_frame_number = physical_to_pfn(physical_addr);
    printf("*********************receive slub\n");
    printf("free physical addr is %x\n",physical_addr);
    printf("page number is %d\n", page_frame_number);
    printf("page addr is %x\n", page_t_pool[page_frame_number].phy_addr);
    printf("page slub index is %d\n", page_t_pool[page_frame_number].slub_index);

    slub_t* tmp = page_t_pool[page_frame_number].slub_next;
    page_t_pool[page_frame_number].slub_next = (slub_t *)(physical_addr);
    slub_t* new = (slub_t *)(physical_addr);
    new->next = tmp;
    
    // kmem_cache_arr[page_t_pool[page_frame_number].slub_index].cache_cpu.free_list = page_t_pool[page_frame_number].slub_next;
    page_t_pool[page_frame_number].slub_num++;
    if(page_t_pool[page_frame_number].slub_num == PAGE_SIZE/SLUB_INDEX_TO_SIZE(page_t_pool[page_frame_number].slub_index)){
        printf("/////////////back to buddy!\n");
        if(page_frame_number !=  kmem_cache_arr[page_t_pool[page_frame_number].slub_index].cache_cpu.page->page_index){
            list_remove_chain(&(page_t_pool[page_frame_number].list),&(page_t_pool[page_frame_number].list));
        }
        else{
            kmem_cache_arr[page_t_pool[page_frame_number].slub_index].cache_cpu.free_list = NULL;
            kmem_cache_arr[page_t_pool[page_frame_number].slub_index].cache_cpu.page = NULL;
        }
        init_page(page_frame_number);
        free_page(page_t_pool[page_frame_number].phy_addr);
    }
}

void buddy_sys_test(){
        // page_t* p = get_pages_from_list(5);
    // print_buddy_info();
    // printf("------\n");

    // page_t* p2 = get_pages_from_list(3);
    // print_buddy_info();
    // printf("------\n");

    // page_t* p3 = get_pages_from_list(2);
    // print_buddy_info();
    // printf("------\n");

    // page_t* p4 = get_pages_from_list(2);
    // print_buddy_info();
    // printf("------\n");

    // page_t* p5 = get_pages_from_list(2);
    // print_buddy_info();
    // printf("------\n");

    // free_page(p2->phy_addr);
    // print_buddy_info();
    // printf("------\n");

    // free_page(p3->phy_addr);
    // print_buddy_info();
    // printf("------\n");


    // free_page(p4->phy_addr);
    // print_buddy_info();
    // printf("------\n");

    // free_page(p5->phy_addr);
    // print_buddy_info();
    // printf("------\n");

    // free_page(p->phy_addr);
    // print_buddy_info();
    // printf("------\n");

    // page_t* p = get_pages_from_list(1);
    // print_buddy_info();
    // printf("------\n");

    // page_t* p2 = get_pages_from_list(1);
    // print_buddy_info();
    // printf("------\n");
}

void test(int test_all){
    unsigned long c1,c2,c3,c4,c5,c6;
    if(test_all)
        c1 = allocate_memory(9000);


    unsigned long a = allocate_memory(256);

    unsigned long a2 = allocate_memory(513);
    unsigned long a3 = allocate_memory(513);
    unsigned long a4 = allocate_memory(513);

    if(test_all)
        receive_memory(c1);
    if(test_all)
        c2 = allocate_memory(8000);

    unsigned long a5 = allocate_memory(513);
    unsigned long a6 = allocate_memory(513);

    receive_memory(a3);
    receive_memory(a2);
    receive_memory(a4);
    receive_memory(a5);

    if(test_all)
        receive_memory(c2);


    unsigned long b1 = allocate_memory(513);
    unsigned long b2 = allocate_memory(513);
    unsigned long b3 = allocate_memory(513);
    receive_memory(b3);
    unsigned long b4 = allocate_memory(513);
    receive_memory(a6);

    unsigned long a7 = allocate_memory(513);
    unsigned long a8 = allocate_memory(513);
    unsigned long a9 = allocate_memory(513);
    unsigned long a10 = allocate_memory(513);
    unsigned long a11 = allocate_memory(513);

    receive_memory(a);

    receive_memory(a8);
    receive_memory(a7);
    
    if(test_all)
        c3 = allocate_memory(10000);

    receive_memory(a9);
    receive_memory(a10);
    
    if(test_all)
        receive_memory(c3);

    receive_memory(a11);

    if(test_all)
        c4 = allocate_memory(15000);

    receive_memory(b1);
    receive_memory(b2);
    
    if(test_all)
        receive_memory(c4);

    if(test_all)
        c5 = allocate_memory(30000);
    if(test_all)
        c6 = allocate_memory(25000);
    
    receive_memory(b4);

    if(test_all)
        receive_memory(c5);
    if(test_all)
        receive_memory(c6);
}

void test2(){
    unsigned long b3 = allocate_memory(513);
    receive_memory(b3);

    unsigned long a7 = allocate_memory(513);
    receive_memory(a7);


    unsigned long a8 = allocate_memory(513);
    unsigned long a9 = allocate_memory(513);
    unsigned long a10 = allocate_memory(513);
    unsigned long a11 = allocate_memory(513);

    receive_memory(a8);
    receive_memory(a9);
    receive_memory(a10);
    receive_memory(a11);

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
        page_t_pool[p].slub_next = NULL;
        page_t_pool[p].slub_num = -1;
        page_t_pool[p].slub_index = -1;
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
    init_kmalloc_caches();
    test(1);
    print_buddy_info();
}