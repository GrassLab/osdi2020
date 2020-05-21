#ifndef __MMU_H__
#define __MMU_H__

typedef enum{
	kernel,
	user
}mem_mode;

typedef struct{
	int alloc;			
	mem_mode mmode;
}page_t;

typedef struct{
	unsigned long long PGD;
	unsigned long long PUD;
	unsigned long long PMD;
	unsigned long long PTE;

	int allocNum;
}user_page_info_t;

void mmu_info_init(void);
int user_paging_init(user_page_info_t *pginfo);
unsigned long long phyAddr2virtAddr(unsigned long long phyAddr);


// mmu.S
void set_ttbr0(unsigned long long PGD);
void copy_rd(unsigned long long virtAddr);
void set_user_rd(user_page_info_t *pginfo);


#endif