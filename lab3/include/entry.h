
#ifndef _ENTRY_H
#define _ENTRY_H

#define S_FRAME_SIZE			256 		// size of all saved registers 

#define SYNC_INVALID_EL2t		0 
#define IRQ_INVALID_EL2t		1 
#define FIQ_INVALID_EL2t		2 
#define ERROR_INVALID_EL2t		3 

#define SYNC_INVALID_EL2h		4 
#define IRQ_INVALID_EL2h		5 
#define FIQ_INVALID_EL2h		6 
#define ERROR_INVALID_EL2h		7 

#define SYNC_INVALID_EL0_64	    8 
#define IRQ_INVALID_EL0_64	    9 
#define FIQ_INVALID_EL0_64		10 
#define ERROR_INVALID_EL0_64	11 

#define SYNC_INVALID_EL0_32		12 
#define IRQ_INVALID_EL0_32		13 
#define FIQ_INVALID_EL0_32		14 
#define ERROR_INVALID_EL0_32	15 
#endif