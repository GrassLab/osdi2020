#include "uart.h"
#include "str.h"
#include "mailbox.h"
#include "lfb.h"
#include "exc.h"
#include "syscall.h"

#define PAGE_SIZE 4096

#define PAGE_POW 9
#define NUM_PAGE (1<<PAGE_POW)
#define LINK_LIST_SIZE (16)
#define ALLOCATED 1
#define NO_ALLOCATED 0
#define USAGE 2

typedef struct link_list{
	unsigned long long pgnum;
    struct link_list* next;
    unsigned long long ppow;
} link_list;

typedef struct obj_struct{
	unsigned long long obj_size;
    unsigned long long bitmap_size; //number of bit
    struct obj_struct* next;
} obj_struct;

#define REAL_OBJ_PAGE_SIZE (PAGE_SIZE-sizeof(link_list)-sizeof(obj_struct))
#define REAL_PAGE_SIZE (PAGE_SIZE-sizeof(link_list))


/*typedef struct page{
    char used;
} page;*/

__attribute__((section(".userspace"))) link_list *all_link[PAGE_POW+1]; //link start array
__attribute__((section(".userspace"))) obj_struct *obj_list_start; //link start array
__attribute__((section(".userspace"))) char page_frame[NUM_PAGE][PAGE_SIZE];
//__attribute__((section(".userspace"))) page all_page[262144];

void link_init()
{
	for(int i=0;i<PAGE_POW;i++)
    {
        all_link[i] = 0;
    }
    obj_list_start = 0;
    // put the list struct in each chunk head
    all_link[PAGE_POW] = (link_list*)page_frame[0]; //set the max page
    all_link[PAGE_POW]->pgnum = 0;
    all_link[PAGE_POW]->next = 0;
    all_link[PAGE_POW]->ppow = PAGE_POW;
    //my_printf("%x \r\n", (unsigned long long)all_link[PAGE_POW]);
}

char* mmap(unsigned long long size)
{
    int pow2=1, tmp=0;
    link_list* tmp_next, *final_ret = 0;
    size += sizeof(link_list);
    // calculate the min page size (pow of 2 * PAGE_SIZE) 
    for(int i=0; i<=PAGE_POW; i++)
    {
        if( ((pow2 << i)*(PAGE_SIZE)) >= size)
        {
            tmp = i;
            break;
        }
    }


    // no this size's list
    if(all_link[tmp] == 0)
    {
        int i=tmp, succ=0;
        //first find the exist smallest size's list
        for(;i<=PAGE_POW; i++)
        {
            if(all_link[i] != 0)
            {
                succ = 1;
                break;
            }
        }

        // if found
        if(succ)
        {
            for(int j=i; j>tmp; j--)
            {
                tmp_next = 0;
                //to spilt a chunk, we need to move the next to this one, save the next pointer
                if(all_link[j]->next != 0)
                {
                    tmp_next = all_link[j]->next;
                }

                // new chunk point to head
                all_link[j-1] = all_link[j];
                int sec_page_num = ((all_link[j]->pgnum) + (1<<(j-1))); // cal second chunk
               
                all_link[j-1]->next = (link_list*)page_frame[sec_page_num]; //assign second chunk
                all_link[j-1]->next->pgnum = sec_page_num;
                all_link[j-1]->next->next = 0;
                all_link[j-1]->next->ppow = tmp;
                
                all_link[j-1]->ppow = tmp;
                
                //recover old next chunk.
                if(tmp_next != 0)
                {
                    all_link[j] = tmp_next;
                }
                else
                {
                    all_link[j] = 0;
                }
            }
        }
    }

    if(all_link[tmp] != 0)
    {
        final_ret = all_link[tmp];
        final_ret->ppow = tmp;
        //if chunk usage, remove it from list
        if(all_link[tmp]->next != 0)
        {
            all_link[tmp] = all_link[tmp]->next;
        }
        else
        {
            all_link[tmp] = 0;
        }
        //my_printf("%d\r\n", *(unsigned long long*)(((unsigned long long*)page_frame[0])[1])   );
        //my_printf("%x \r\n" , (unsigned long long)final_ret+sizeof(link_list));
        return (char *)((unsigned long long)final_ret + sizeof(link_list));
    }
    else
    {
        return 0;
    }

}

int merge_check(link_list* chunk_check)
{
    uart_puts("in merge check\r\n");
    int now_ppow = chunk_check->ppow, gap, merged=0;
    link_list *now, *low, *prev=0, *next_level_tail; //*high
    now = all_link[now_ppow];
    do{
        gap = chunk_check->pgnum - now->pgnum;
        gap = (gap > 0) ? gap : -gap;
       
        if(gap == (1<<now_ppow)){
            merged = 1;
            //high = (now->pgnum > chunk_check->pgnum) ? now : chunk_check;
            low = (now->pgnum > chunk_check->pgnum) ? chunk_check : now;
            if((low->pgnum/(1<<now_ppow)) % 2 != 0)
            {
                prev = now;
                now = now->next;
                continue;
            }
            low->ppow++;
            low->next = 0;
            if(now==all_link[now_ppow])
            {
                all_link[now_ppow] = now->next;
            }
            else
            {
                if(prev != 0)
                {
                    prev->next = now->next;
                }
            }
            if(all_link[now_ppow+1] == 0)
            {
                all_link[now_ppow+1] = low;
            }
            else{
                if(merge_check(low) == 0){
                    next_level_tail = all_link[now_ppow+1];
                    while(next_level_tail->next != 0)
                    {
                        next_level_tail = next_level_tail->next;
                    }
                    next_level_tail->next = low;
                }
            }
            break; //the least have been merged;
        }
        prev = now;
        now = now->next;
    }while(now != 0);
    
    return merged;
}

void free(void *fre)
{
    link_list* chunk_head = (link_list*)((unsigned long long)fre - sizeof(link_list)), *tmp_next;  
    chunk_head->next = 0;
    //my_printf("pgnum: %d , now: %x ,next: %x , ppow: %d\r\n", chunk_head->pgnum, (unsigned long long)chunk_head, (unsigned long long)chunk_head->next, chunk_head->ppow);
    int ppow = chunk_head->ppow;
    if(all_link[ppow] == 0)
    {
        all_link[ppow] = chunk_head;
        all_link[ppow]->next = 0;
    }
    else
    {
        if(merge_check(chunk_head) == 0)
        {
            tmp_next = all_link[ppow];
            //find list tail
            while(tmp_next->next != 0)
            {
                tmp_next = tmp_next->next; 
            }
            tmp_next->next = chunk_head;
        }
        //my_printf("gap : %d %d\r\n", tmp_next->pgnum, tmp_next->next->pgnum);
    }
}

void print_link_header_state()
{
    link_list* now;
    uart_puts("\r\n----------------link state-----------------\r\n");
    for(int i=0;i<=PAGE_POW;i++)
    {
        my_printf("%d: ", 1<<i);
        now = all_link[i];
        while(now != 0)
        {
            my_printf("%d, ", now->pgnum);
            now=now->next;
        }
        uart_puts(" | ");
    }
    uart_puts("\r\n---------------link state----------------\r\n");
}

int registe_object(unsigned long long size)
{
    if(size >= REAL_OBJ_PAGE_SIZE)
    {
        return -1;
    }
    char *space = mmap(REAL_PAGE_SIZE);
    link_list *pagep = (link_list*)((unsigned long long)space - sizeof(link_list));
    obj_struct *obj_dec = (obj_struct*)space;
    obj_dec->obj_size = size;
    obj_dec->bitmap_size = 0;
    return pagep->pgnum;
}

void *object_allocate(int pgnum)
{
    if(pgnum > NUM_PAGE || pgnum < 0 ) return 0; 
    //int realsize = REAL_OBJ_PAGE_SIZE;
    link_list *pagep = (link_list*)page_frame[pgnum];
    obj_struct *obj_dec = (obj_struct*)((unsigned long long)pagep + sizeof(link_list));
    char *bitmap = (char*)((unsigned long long)obj_dec + sizeof(obj_struct));
    int char_size;

    if(obj_dec->bitmap_size == 0) //did't init bookkeeping
    {
        /////////////////////////
        //char_size = bit_size//
        //char_size = bit_size//
        //char_size = bit_size//
        ////////////////////////
        char_size = REAL_OBJ_PAGE_SIZE / obj_dec->obj_size;
        while( (REAL_OBJ_PAGE_SIZE - (obj_dec->obj_size*char_size)) < ( (char_size%8 ==0) ? (char_size/8) : ((char_size/8)+1) ) )
        {
            char_size--;
            if(char_size==0) //should not happend, just in case
                return 0;
        }
        obj_dec->bitmap_size = char_size; //size in bit
        //my_printf("size : %d\r\n", obj_dec->bitmap_size);
        for(int i=0; i < ((char_size/8)+1); i++)
        {
            bitmap[i] = 0; //set 8 bits to 0
        }
        obj_dec->next = 0;

        if(obj_list_start == 0)
        {
            obj_list_start = obj_dec;
        }
        else
        {
            obj_struct* tmp;
            tmp = obj_list_start;
            while(tmp->next!=0) tmp = tmp->next;
            tmp->next = obj_dec;
        }
    }

    char_size = (obj_dec->bitmap_size <= 8) ? 1 : ( (obj_dec->bitmap_size%8 ==0) ? (obj_dec->bitmap_size/8) : ((obj_dec->bitmap_size/8)+1) );
    unsigned long long objpos = (unsigned long long)bitmap + char_size;
    int offset;
    for(int i=0; i < char_size; i++)
    {
        for(int j=7;j>=0;j--)
        {
            offset = i*8 + (7-j);
            //my_printf("offset: %d, mapsize: %d\r\n", offset, obj_dec->bitmap_size);
            if(offset >= obj_dec->bitmap_size) //overflow bitmap
            {
                break;
            }
            if(((bitmap[i] & (1<<j)) >> j)==0)
            {
                bitmap[i] =  bitmap[i] | (1<<j);
                return (void *)(objpos + (obj_dec->obj_size)*offset);
            }
        }
    }
    return 0;
}

void print_obj_bitmap(int pgnum)
{
    link_list *pagep = (link_list*)page_frame[pgnum];
    obj_struct *obj_dec = (obj_struct*)((unsigned long long)pagep + sizeof(link_list));
    _Bool *bitmap = (_Bool*)((unsigned long long)obj_dec + sizeof(obj_struct));
    int char_size = (obj_dec->bitmap_size <= 8) ? 1 :  ( (obj_dec->bitmap_size%8 ==0) ? (obj_dec->bitmap_size/8) : ((obj_dec->bitmap_size/8)+1) );
    my_printf("bit map %d %d: ", pgnum, obj_dec->bitmap_size);

    for(int i=0; i < char_size; i++)
    {
        for(int j=7;j>=0;j--)
        {
            if((i*8 + (7-j)) >= obj_dec->bitmap_size) //overflow bitmap
            {
                break;
            }
            my_printf("%d,", ((bitmap[i] & (1<<j)) >> j));
        }
    }
    my_printf("\r\n");
}

void show_obj_link()
{
    obj_struct *tmp = obj_list_start;
    my_printf("obj link list: ");
    while(tmp->next!=0)
    {
        my_printf("%d, ", tmp->obj_size);
        tmp = tmp->next;
    }
    my_printf("%d, ", tmp->obj_size);
    my_printf("\r\n");
}

_Bool object_free(void* obj)
{
    int pgnum = ((unsigned long long)obj - (unsigned long long)page_frame)>>12;
    link_list *pagep = (link_list*)page_frame[pgnum];
    obj_struct *obj_dec = (obj_struct*)((unsigned long long)pagep + sizeof(link_list));
    if(obj_dec->bitmap_size == 0)
    {
        return 0;
    }
    unsigned char *bitmap = (unsigned char*)((unsigned long long)obj_dec + sizeof(obj_struct));
    int char_size = (obj_dec->bitmap_size <= 8) ? 1 :  ( (obj_dec->bitmap_size%8 ==0) ? (obj_dec->bitmap_size/8) : ((obj_dec->bitmap_size/8)+1) );
    int idx = (  ((unsigned long long)obj - ((unsigned long long)bitmap + char_size)) / obj_dec->obj_size  );
    my_printf("idx : %d\r\n", idx);
    int offset, checked=1;
    for(unsigned int i=0; i < char_size; i++)
    {
        for(unsigned int j=7;j>=0;j--)
        {
            offset = i*8 + (7-j);
            //my_printf("offset: %d, mapsize: %d\r\n", offset, obj_dec->bitmap_size);
            if(offset >= obj_dec->bitmap_size) //overflow bitmap
            {
                break;
            }
            if(offset == idx)
            {
                bitmap[i] =  bitmap[i] & (~(1<<j));
            }
        }
    }
    for(int i=0;i<char_size;i++)
    {
        if(bitmap[i] != 0)
        {
            checked =0;
        }
    }
    if(checked)
    {
        obj_struct *now = obj_list_start, *prev=0;
        do{
            if(now == obj_dec)
            {
                if(prev == 0)
                {
                    obj_list_start = now->next;
                }
                else
                {
                    prev->next = now->next;
                }
                break;
            }
            prev = now;
            now = now->next;
        }while(now != 0);
        free(obj_dec);
    }
    //my_printf("idx: %d\r\n", idx);
    return 0;
}

void object_logout(int pgnum)
{
    link_list *pagep = (link_list*)page_frame[pgnum];
    obj_struct *obj_dec = (obj_struct*)((unsigned long long)pagep + sizeof(link_list));
    if(obj_dec->bitmap_size == 0)
    {
        return;
    }
    obj_dec->bitmap_size = 0;
    obj_struct *now = obj_list_start, *prev=0;
    do{
        if(now == obj_dec)
        {
            if(prev == 0)
            {
                obj_list_start = now->next;
            }
            else
            {
                prev->next = now->next;
            }
            break;
        }
        prev = now;
        now = now->next;
    }while(now != 0);

    free(obj_dec);
}

/*void* varied_allocate(unsigned long long size)
{
    if(size < REAL_OBJ_PAGE_SIZE)
    {
       
    }
    else
    {
        return (void*)mmap(size);
    }
}*/

void main()
{
    uart_init();
    link_init();
    print_link_header_state();
	int a = registe_object(252), b = registe_object(224), c = registe_object(339);
    char *kk = object_allocate(a), *zz =  object_allocate(b), *yy =  object_allocate(c);
    print_link_header_state();
    show_obj_link();
    object_free(kk);
    show_obj_link();
    print_link_header_state();
    object_free(zz);
    show_obj_link();
    print_link_header_state();
    
    /*object_allocate(a);
    print_obj_bitmap(a);
    object_free(kk);
    print_obj_bitmap(a);*/
    /*//map test
    char *a = mmap(28672), *b = mmap(28672), *c = mmap(28672), *d = mmap(28672), *e = mmap(3000), *f = mmap(61440);
    char *w = mmap(28672), *x = mmap(28672), *y = mmap(28672), *z = mmap(28672);

    print_link_header_state();
    free(c);
    print_link_header_state();
    free(a);
    print_link_header_state();
    free(b);
    print_link_header_state();
    free(d);
    print_link_header_state();
    
    free(y);
    print_link_header_state();
    free(w);
    print_link_header_state();
    free(x);
    print_link_header_state();
    free(z);
    print_link_header_state();
    free(e);
    print_link_header_state();
    free(f);
    print_link_header_state();*/

	while(1);
	
}