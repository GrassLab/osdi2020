#include "slab.h"
#include "tools.h"

void init_all_allocator(){
    for(int i=0;i<NUM_ALLOCATOR;i++){
        allocator_used[i]=0;
    }    
}
int init_allocator(int size){
    for(int i=0;i<NUM_ALLOCATOR;i++){
        if(allocator_used[i]==0){      
            uart_puts("[Slab]New Allocator for size:");
            uart_send_int(size);
            uart_puts("\n");  
            allocator_pool[i].len = 0;
            allocator_pool[i].size = size;
            allocator_pool[i].chunk_head = 0;
            allocator_used[i] = 1;
            return i;
        }        
    }
    uart_puts("[Slab]Init Allocator fail!\n");           
    return -1;
}
void show_allocator(int allocator_id){
    uart_puts("\n[Slab]Show Allocator!\n");           
    if(allocator_used[allocator_id]==0){
        uart_puts("[Slab]Unuse Allocator!\n");           
        return;
    }
    if(allocator_pool[allocator_id].len==0){
        uart_puts("[Slab]Allocator null!\n");           
        return;
    }
    uart_puts("[Slab]Allocator Number: ");  
    uart_send_int(allocator_id);         
    uart_puts("\n");
    uart_puts("[Slab]Len: ");  
    uart_send_int(allocator_pool[allocator_id].len);         
    uart_puts("\n");
    
    chunk* head = allocator_pool[allocator_id].chunk_head;    

    while(head!=0){   
        uart_hex(head->addr);         
        uart_puts("->");
        head = head->next;
    }
    uart_puts("0\n");
    
}

int alloc(int allocator_id){
    if(allocator_used[allocator_id]==0){
        uart_puts("[Slab]Unuse Allocator!\n");           
        return -1;
    }
    //alloc from buddy system
    if(allocator_pool[allocator_id].len==0){
        int order = 0;
        int base_addr = pfn2phy(buddy_alloc(order));
        int new_chunk_num = (1<<order)*PAGE_SIZE / allocator_pool[allocator_id].size;

        chunk* head = (chunk*)base_addr;
        head->addr = base_addr;
        head->size = allocator_pool[allocator_id].size;
        head->next = 0;
        allocator_pool[allocator_id].chunk_head = head;
        
        for(int i=1;i<new_chunk_num;i++){
            base_addr+=allocator_pool[allocator_id].size;
            chunk* temp = (chunk*)base_addr;
            temp->addr = base_addr;
            temp->size = allocator_pool[allocator_id].size;
            temp->next = 0;

            head->next = temp;
            head = head->next;
        }
        allocator_pool[allocator_id].len += new_chunk_num;
    }
    //slab remain
    if(allocator_pool[allocator_id].len>0){
        int ret = allocator_pool[allocator_id].chunk_head->addr;
        allocator_pool[allocator_id].chunk_head = allocator_pool[allocator_id].chunk_head->next;
        allocator_pool[allocator_id].len--;
        return ret;
    }
    uart_puts("[Slab]Unknown error!\n");           
    return -1;
}

void free_alloc(int allocator_id, int addr){
    if(allocator_used[allocator_id]==0){
        uart_puts("[Slab]Unuse Allocator!\n");           
        return -1;
    }
    if(addr==-1){
        uart_puts("[Slab]Invalid address!\n");           
        return -1;
    }
    uart_puts("\n[Slab]Free Allocator at: ");   
    uart_hex(addr);        
    uart_puts("\n");
    chunk* temp = (chunk*)addr;
    temp->addr = addr;
    temp->size = allocator_pool[allocator_id].size;
    temp->next = 0;

    if(allocator_pool[allocator_id].len!=0)
        temp->next = allocator_pool[allocator_id].chunk_head;
    allocator_pool[allocator_id].chunk_head = temp;
    allocator_pool[allocator_id].len++;
}

void init_var_allocator(){
    uart_puts("[Slab]init_var_alloc!\n");   
    int sizes[] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
    int num = 10;
    for(int i=0;i<num;i++){
        init_allocator(sizes[i]);
    }    
}

int find_id(int size){
    int alloc_id = -1;
    int minn = 999999;
    for(int i=0;i<NUM_ALLOCATOR;i++){
        if(allocator_used[i]==0)continue;
        if(allocator_pool[i].size < size)continue;
        int dis = (allocator_pool[i].size - size);        
        if(dis < minn){
            alloc_id = i;
            minn = dis;
        }
    }
    return alloc_id;
}
int var_alloc(int size){
    int alloc_id = find_id(size);
    if(alloc_id==-1){
        uart_puts("[Slab]Can't find suitable Allocator!\n"); 
        return -1;
    }
    uart_puts("[Slab]Find Allocator for size: ");   
    uart_send_int(size);
    uart_puts(" in: ");   
    uart_send_int(allocator_pool[alloc_id].size);
    uart_puts(" Allocator id: ");   
    uart_send_int(alloc_id);        
    uart_puts("\n");
    // int ret = 0;
    int ret = alloc(alloc_id);
        
    return ret;
}
void var_free(int address, int size){
    if(address==-1){
        uart_puts("[Slab]Invalid address!\n"); 
        return;
    }
    int alloc_id = find_id(size);
    show_allocator(alloc_id);
    free_alloc(alloc_id, address);
    show_allocator(alloc_id);
}