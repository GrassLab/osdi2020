#define DEVICE_BASE             0x3F000000 

#define VA_START 			    0xffff000000000000

#define PHYS_MEMORY_SIZE 		0x40000000	

#define PAGE_MASK			    0xfffffffffffff000
#define PAGE_SHIFT	 		    12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   			(1 << PAGE_SHIFT)	
#define SECTION_SIZE			(1 << SECTION_SHIFT)	

#define LOW_MEMORY              (2 * SECTION_SIZE)
#define HIGH_MEMORY             DEVICE_BASE

#define PAGING_MEMORY 			(HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES 			(PAGING_MEMORY/PAGE_SIZE)

#define PTRS_PER_TABLE			(1 << TABLE_SHIFT)

#define PGD_SHIFT		        PAGE_SHIFT + 3*TABLE_SHIFT
#define PUD_SHIFT			    PAGE_SHIFT + 2*TABLE_SHIFT
#define PMD_SHIFT			    PAGE_SHIFT + TABLE_SHIFT

#define PG_DIR_SIZE			    (3 * PAGE_SIZE)

// void set_TCR_EL1();
// void set_MAIR_EL1();
// void set_identity_mapping();