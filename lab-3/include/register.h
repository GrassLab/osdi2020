#ifndef REGISTER_H
#define REGISTER_H

#define HCR_RW	    			(1 << 31)
#define HCR_VALUE			    HCR_RW

#define SPSR_MASK_ALL 			(7 << 6)
#define SPSR_EL2h			    (9 << 0)
#define SPSR_EL1h			    (5 << 0)
#define SPSR_EL0t			    (0 << 0)
#define SPSR_VALUE			    (SPSR_MASK_ALL | SPSR_EL1h)

#define EL1_SPSR_MASK           (1 << 6)
#define EL1_SPSR_EL0		    (15 << 0)
#define EL1_SPSR_VALUE          (EL1_SPSR_MASK & ~EL1_SPSR_EL0)

#define PAGE_SHIFT	 		    12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   			(1 << PAGE_SHIFT)
#define SECTION_SIZE			(1 << SECTION_SHIFT)	

#define EL1_LOW_MEMORY          (2 * SECTION_SIZE)
#define EL0_LOW_MEMORY          (4 * SECTION_SIZE)

#endif