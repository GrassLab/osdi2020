#include "mmu_size.h"
#include "mmu.h"


page_t page[FRAME_NUM];

void mmu_info_init(){
	// kernel space
	for(int i=0; i<USER_SPACE_START_ID; i++){
		page[i].alloc = 1;
		page[i].mmode = kernel;
	}

	for(int i=USER_SPACE_START_ID; i<USER_SPACE_END_ID; i++){
		page[i].alloc = 0;
		page[i].mmode = user;
	}

	// // Peripherals
	for(int i=USER_SPACE_END_ID; i<FRAME_NUM; i++){
		page[i].alloc = 1;
		page[i].mmode = kernel;
	}	
}

unsigned long long virtAddr2phyAddr(unsigned long long virtAddr){
	return (virtAddr & 0x0000FFFFFFFFFFFF);
}

unsigned long long phyAddr2virtAddr(unsigned long long phyAddr){
	return (phyAddr & 0xFFFF000000000000);
}

int phyAddr2PFN(unsigned long long phyAddr){
	return ( phyAddr>> 12 );
}

unsigned long long PFN2phyAddr(int PFN){
	return (PFN << 12);
}

int page_alloc(){
	for(int PFN=USER_SPACE_START_ID; PFN<USER_SPACE_END_ID; PFN++){
		if(page[PFN].alloc==0 && page[PFN].mmode==user){
			page[PFN].alloc = 1;
			return PFN;
		}
	}

	return -1;
}

void page_free(int PFN){
	if(PFN>FRAME_NUM || PFN<0) return;
	page[PFN].alloc = 0;
}

void clear_page(int PFN){
	unsigned long long phyAddr = PFN2phyAddr(PFN);
	unsigned long long virtAddr = phyAddr2virtAddr(phyAddr);

	for(int i=0; i< (USER_FRAME_SIZE<<3); i++){
		*(volatile unsigned long long *)virtAddr = 0;
		virtAddr += 8;
	}
}

void user_page_table_init(int PGD_PFN, int PUD_PFN, int PMD_PFN, int PTE_PFN, int alloc_PFN, user_page_info_t *pginfo){
	unsigned long long PGD_addr = PFN2phyAddr(PGD_PFN);
	unsigned long long PUD_addr = PFN2phyAddr(PUD_PFN);
	unsigned long long PMD_addr = PFN2phyAddr(PMD_PFN);
	unsigned long long PTE_addr = PFN2phyAddr(PTE_PFN);
	unsigned long long alloc_addr = PFN2phyAddr(alloc_PFN);

	// fill PGD
	*((volatile unsigned long long *)phyAddr2virtAddr(PGD_addr)) = ( PUD_addr | BOOT_PGD_ATTR );
	// fill PUD
	*((volatile unsigned long long *)phyAddr2virtAddr(PUD_addr)) = ( PMD_addr | BOOT_PUD_ATTR );
	// fill PMD
	*((volatile unsigned long long *)phyAddr2virtAddr(PMD_addr)) = ( PTE_addr | USER_PMD_ATTR );
	// fill PTE
	*((volatile unsigned long long *)phyAddr2virtAddr(PTE_addr)) = ( alloc_addr | USER_PTE_ATTR );

	pginfo->PGD = PGD_addr;
	pginfo->PUD = PUD_addr;
	pginfo->PMD = PMD_addr;
	pginfo->PTE = PTE_addr;
}

int user_paging_init(user_page_info_t *pginfo){
	int demandPageNum = 5;
	int getPage[5] = {0};
	int getPageNum = 0;
	for(int i=0; i<demandPageNum; i++){
		int pageId = page_alloc();

		if(pageId != -1){
			getPage[getPageNum] = pageId;
			getPageNum++;
		}
		else break;
	}

	if(getPageNum!=5){
		for(int i=0; i<getPageNum; i++) page_free(getPage[i]);
		return -1;
	}else{
		for(int i=0; i<getPageNum; i++) clear_page(getPage[i]);
	}
	pginfo->allocNum = 1;

	user_page_table_init(getPage[0], getPage[1], getPage[2], getPage[3], getPage[4], pginfo);

	return 1;
}

int user_page_new(user_page_info_t *pginfo){
	int new_PFN = page_alloc();
	if(new_PFN == -1) return -1;

	unsigned long long new_page_phyAddr = PFN2phyAddr(new_PFN);
	*((volatile unsigned long long*)phyAddr2virtAddr(pginfo->PTE + pginfo->allocNum*8)) = ( new_page_phyAddr | USER_PTE_ATTR );
	pginfo->allocNum++;

	return 1;

}

void set_user_rd(user_page_info_t *pginfo){
	unsigned long long PTE_phyAddr = pginfo->PTE;
	unsigned long long PTE_virtAddr = phyAddr2virtAddr(PTE_phyAddr);
	unsigned long long code_phyAddr = *((volatile unsigned long long *)PTE_virtAddr);
	unsigned long long code_virtAddr = phyAddr2virtAddr(code_phyAddr);

	set_ttbr0(pginfo->PGD);
	copy_rd(code_virtAddr);
}
