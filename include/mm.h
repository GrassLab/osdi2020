#define DEVICE_BASE             0x3F000000 

#define VA_START 			    0xffff000000000000

#define PHYS_MEMORY_SIZE 		0x40000000	

#define PAGE_MASK			    0xfffffffffffff000
#define PAGE_SHIFT	 		    12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)
// 4 KB
#define PAGE_SIZE   			(1 << PAGE_SHIFT)	
// 2 MB
#define SECTION_SIZE			(1 << SECTION_SHIFT)	

#define PTRS_PER_TABLE			(1 << TABLE_SHIFT)

#define HIGH_MEMORY             DEVICE_BASE // 0x3F00 0000
#define LOW_MEMORY              (3 * SECTION_SIZE) // (3 * 2^21) = 6M
// 
#define PAGING_MEMORY 			(HIGH_MEMORY - LOW_MEMORY)
// 0x3efff
// #define PAGING_PAGES 			(PAGING_MEMORY/PAGE_SIZE) 

#define LAST_AVAIL_PAGE         (HIGH_MEMORY/PAGE_SIZE)
#define FIRST_AVAIL_PAGE        (LOW_MEMORY/PAGE_SIZE)


// 39
#define PGD_SHIFT		        (PAGE_SHIFT + 3 * TABLE_SHIFT)
// 30
#define PUD_SHIFT			    (PAGE_SHIFT + 2 * TABLE_SHIFT)
// 21
#define PMD_SHIFT			    (PAGE_SHIFT +     TABLE_SHIFT)
// 12
#define PTE_SHIFT               (PAGE_SHIFT)

#define PG_DIR_SIZE			    (3 * PAGE_SIZE)

// void set_TCR_EL1();
// void set_MAIR_EL1();
// void set_identity_mapping();


