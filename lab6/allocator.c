#include "mystd.h"
#include "buddy.h"
#include "uart.h"
#include "allocator.h"

fix_allocator fix_alloc_pool[32];
int fix_allocator_gid = 0;

dynamic_allocator dynamic_alloc_pool[32];
int dynamic_allocator_gid = 0;

void show_allocator_alloc_message(unsigned long long addr, int obj_size){
	char buf[16] = {0};

	uart_puts("[Allocator] allocate object ");
	ullToStr(obj_size, buf);
	uart_puts(buf);
	uart_puts(" bytes at ");
	ullToStr_hex(addr, buf);
	uart_puts(buf);
	uart_puts(" \n");
}

void show_allocator_free_message(unsigned long long addr, int obj_size){
	char buf[16] = {0};

	uart_puts("[Allocator] free object ");
	ullToStr(obj_size, buf);
	uart_puts(buf);
	uart_puts(" bytes at ");
	ullToStr_hex(addr, buf);
	uart_puts(buf);
	uart_puts(" \n");
}

unsigned long long PFN2ADDR(int PFN){
	return ( (unsigned long long)(PFN << 12 | 0xFFFF000000000000) ) ;
}


void fix_allocator_request_page(fix_allocator *self){
	int PFN = buddy_alloc(1);
	
	int thisPFN_Id = self->PFN_num;
	self->mem_pool[thisPFN_Id] = self->mem_pool_init;
	self->PFN[thisPFN_Id] = PFN;
	self->PFN_num ++ ;
}

// Min is 64B
fix_allocator* fix_allocator_init(int size){
	int thisId = fix_allocator_gid;
	fix_allocator_gid++;

	fix_alloc_pool[thisId].PFN_num = 0;
	fix_alloc_pool[thisId].obj_size = size;
	fix_alloc_pool[thisId].mem_pool_init = (FIX_ALLOCATOR_MEM_POOL_FULL << ( size==64?63:PAGE_SIZE/size )) ;

	fix_allocator_request_page(&fix_alloc_pool[thisId]);

	return &fix_alloc_pool[thisId];
}

int find_mem_pool(unsigned long long *pool){
	for(int i=0; i<64; i++){
		int mask = 1 << i;
		if( (mask & *pool) == 0 ){
			*pool = *pool | mask;
			return i;
		}
	}

	return -1;
}

void* fix_alloc(fix_allocator *self){
	int PFN_id = self->PFN_num-1;
	int offset = find_mem_pool(&self->mem_pool[PFN_id]);
	offset *= self->obj_size;

	unsigned long long retAddr = PFN2ADDR(self->PFN[PFN_id]) + offset;
	show_allocator_alloc_message(retAddr, self->obj_size);

	return (void*)(retAddr);
}

int fix_free(fix_allocator *self, void* addr){
	unsigned long long phyAddr = ((unsigned long long)addr & 0x0000FFFFFFFFFFFF);
	int PFN = phyAddr >> 12;
	int offset = (phyAddr & 0x0000000000000FFF)/self->obj_size ;
	
	int PFN_id = -1;
	for(int i=0; i<self->PFN_num; i++){
		if(self->PFN[i] == PFN){
			PFN_id = i;
			break;
		}
	}

	if(PFN_id == -1) return 0;

	int mask = ~(1<<offset);
	self->mem_pool[PFN_id] &= mask;

	show_allocator_free_message((unsigned long long)addr, self->obj_size);

	return 1;
}

dynamic_allocator* dynamic_allocator_init(){
	int thisId = dynamic_allocator_gid;
	dynamic_allocator_gid++;

	dynamic_alloc_pool[thisId].fix_allocator_512 = fix_allocator_init(512);
	dynamic_alloc_pool[thisId].fix_allocator_2048 = fix_allocator_init(2048);
	dynamic_alloc_pool[thisId].fix_allocator_4096 = fix_allocator_init(4096);

	return &dynamic_alloc_pool[thisId];
}

void* dynamic_alloc(dynamic_allocator *self, int req_size){
	void* retAddr;
	if(req_size <= 512) retAddr = fix_alloc(self->fix_allocator_512);
	else if(req_size <= 2048) retAddr = fix_alloc(self->fix_allocator_2048);
	else if(req_size <= 4096) retAddr = fix_alloc(self->fix_allocator_4096);
	else{
		int PFN = buddy_alloc( (req_size/4096) + (req_size%4096?1:0) );
		retAddr = (void*)PFN2ADDR(PFN);
	}


	return retAddr;
}

void dynamic_free(dynamic_allocator *self, void* addr){
	if(fix_free(self->fix_allocator_512, addr)) return;
	else if (fix_free(self->fix_allocator_2048, addr)) return;
	else if (fix_free(self->fix_allocator_4096, addr)) return;
	else{
		unsigned long long phyAddr = ((unsigned long long)addr & 0x0000FFFFFFFFFFFF);
		int PFN = phyAddr >> 12;
		buddy_free(PFN);
	}
}