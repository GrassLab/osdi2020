#ifndef _MMU_H
#define _MMU_H

#define MM_TYPE_PAGE_TABLE		    0b11
#define MM_TYPE_PAGE 			    0b11
#define MM_TYPE_BLOCK			    0b01
#define MM_ACCESS			        (1 << 10)
#define MM_ACCESS_PERMISSION		(1 << 6) 
#define MM_READONLY                 (1 << 7)
#define MM_NON_EXEC_EL0             (1UL << 54)

/*
 * Memory region attributes:
 *
 *   n = AttrIndx[2:0]
 *			n	MAIR
 *   DEVICE_nGnRnE	000	00000000
 *   NORMAL_NC		001	01000100
 */
// #define MT_DEVICE_nGnRnE 		    0x0
// #define MT_NORMAL_NC			    0x1
// #define MT_DEVICE_nGnRnE_FLAGS		0x00
// #define MT_NORMAL_NC_FLAGS  		0x44
// #define MAIR_VALUE			        (MT_DEVICE_nGnRnE_FLAGS << (8 * MT_DEVICE_nGnRnE)) | (MT_NORMAL_NC_FLAGS << (8 * MT_NORMAL_NC))
#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1
#define MAIR_CONFIG_DEFAULT ( \
        (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
        (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )


#define MMU_FLAGS	 		        (MM_TYPE_PAGE | (MAIR_IDX_NORMAL_NOCACHE << 2) | MM_ACCESS)	
#define MMU_DEVICE_FLAGS		    (MM_TYPE_PAGE | (MAIR_IDX_DEVICE_nGnRnE << 2) | MM_ACCESS)	
#define MMU_PTE_FLAGS			    (MM_TYPE_PAGE | (MAIR_IDX_NORMAL_NOCACHE << 2) | MM_ACCESS | MM_ACCESS_PERMISSION)	

// #define TCR_T0SZ			        (64 - 48) 
// #define TCR_T1SZ			        ((64 - 48) << 16)
// #define TCR_TG0_4K			        (0 << 14)
// #define TCR_TG1_4K			        (2 << 30)
// #define TCR_VALUE			        (TCR_T0SZ | TCR_T1SZ | TCR_TG0_4K | TCR_TG1_4K)
#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
#define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))
#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)



#endif
