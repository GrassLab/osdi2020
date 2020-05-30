#include "mm.h"
#include "config.h"

void init_buddy_sys(){
	for(int i=0;i<MAX_ORDER;i++){
		INIT_LIST_POINTER(&page_buddy[i]);
	}
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
}