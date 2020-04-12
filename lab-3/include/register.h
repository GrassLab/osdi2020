#ifndef REGISTER_H
#define REGISTER_H

#define HCR_RW	    			(1 << 31)
#define HCR_VALUE			    HCR_RW

#define SPSR_MASK_ALL 			(7 << 6)
#define SPSR_EL1h			    (5 << 0)
#define SPSR_VALUE			    (SPSR_MASK_ALL | SPSR_EL1h)


#endif