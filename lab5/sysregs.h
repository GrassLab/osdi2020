#ifndef _SYSREGS_H
#define _SYSREGS_H


#define HCR_RW	    		(1 << 31)
#define HCR_VALUE			HCR_RW

#define SPSR_MASK_DAIF 		(0b1111 << 6)
#define SPSR_M_EL1h 		(0b0101)
#define SPSR_EL2_VAL 		(SPSR_MASK_DAIF | SPSR_M_EL1h)

#define SPSR_M_EL0			(0b000)
#define SPSR_EL1_VAL 		(SPSR_MASK_DAIF | SPSR_M_EL0)


#endif