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
	
	for(unsigned int i = page_frame_number; i < (page_frame_number + (1 << page_t_pool[page_frame_number].order)); i++){
		page_t_pool[i].used = NOT_USED;
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
        page_t* p = get_pages_from_list(5);
        return p->phy_addr;
    }
    return get_slub(size);
}

void print_slub_info(){
    for(int i = 0 ; i < SLUB_NUMBER; i++){
        printf("--------------------\n");
        printf("now slub size is: %d\n", SLUB_INDEX_TO_SIZE(i));
        slub_t* init_ptr = kmem_cache_arr[i].cache_cpu.free_list;
        while(init_ptr != NULL){
            printf("now addr is: %x\n", init_ptr->next);
            init_ptr = init_ptr->next;
        }
    }
}

void split_page_to_slub(page_t* page, int slub_index){
    int size = SLUB_INDEX_TO_SIZE(slub_index);
    unsigned long start_addr = page->phy_addr;
    page->slub_next = (slub_t *)(start_addr);
    kmem_cache_arr[slub_index].cache_cpu.free_list = page->slub_next;

    slub_t * tmp;
    int slub_num = PAGE_SIZE/size;
    page->slub_num = slub_num;
    for(int i = 0; i < slub_num -1; i++){
        tmp = (slub_t *)(start_addr + i*size);
        printf("######################\n");
        printf("split page addr is:%x\n", tmp);
        tmp->next = (slub_t *)(start_addr + (i+1)*size);
    }
    tmp = (slub_t *)(start_addr + (slub_num-1)*size);
    printf("######################\n");
    printf("split page addr is:%x\n", tmp);
    tmp->next = NULL;
}

void init_kmalloc_caches(){
    for(int s = 0; s < SLUB_NUMBER; s++){
        kmem_cache_arr[s].cache_cpu.free_list = NULL;
        kmem_cache_arr[s].cache_cpu.free_list->next = NULL;
        kmem_cache_arr[s].cache_cpu.page = NULL;
        kmem_cache_arr[s].cache_cpu.partial = NULL;
    }
}

unsigned long get_one_slub(int slub_index){
    slub_t* slub = kmem_cache_arr[slub_index].cache_cpu.free_list;
    printf("######################\n");
    // printf("allocate slub addr is: %x\n", slub->phy_addr);
    printf("allocate slub addr is: %x\n", slub);
    kmem_cache_arr[slub_index].cache_cpu.free_list = slub->next;
    kmem_cache_arr[slub_index].cache_cpu.page->slub_num--;
    return slub;
}

unsigned long get_slub(int size){
    int slub_index = slub_size_to_index(size);
    printf("slub index is %d\n",slub_index);
    if(kmem_cache_arr[slub_index].cache_cpu.free_list == NULL){
        list_add_tail(&(kmem_cache_arr[slub_index].cache_cpu.page->list),&kmem_cache_arr[slub_index].cache_cpu.partial);
        printf("%d slub is empty!\n", SLUB_INDEX_TO_SIZE(slub_index));
        page_t* p = get_pages_from_list(1);
        kmem_cache_arr[slub_index].cache_cpu.page = p;
        print_buddy_info();
        split_page_to_slub(p, slub_index);
    }
    return get_one_slub(slub_index);
}

void test(){
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

    // split_page_to_slub(p, 6);
    // split_page_to_slub(p2, 8);

    // print_slub_info();

    init_kmalloc_caches();

    printf("get addr is %x\n",get_slub(256));

    printf("get addr is %x\n",get_slub(257));
    printf("get addr is %x\n",get_slub(320));
    printf("get addr is %x\n",get_slub(300));
    printf("get addr is %x\n",get_slub(259));
    printf("get addr is %x\n",get_slub(512));
    
    printf("get addr is %x\n",get_slub(513));
    printf("get addr is %x\n",get_slub(513));
    printf("get addr is %x\n",get_slub(513));
    printf("get addr is %x\n",get_slub(513));
    printf("get addr is %x\n",get_slub(513));
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
        page_t_pool[p].slub_num = 0;
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
    test();
}