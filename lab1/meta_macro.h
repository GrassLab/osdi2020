#ifndef __META_MACRO_H__
#define __META_MACRO_H__

#define UNUSED(x) (void)(x)
#define PERIPHERAL_BUS_BASE 0x7E000000
#define PERIPHERAL_PHYSICAL_BASE 0x3F000000
#define PERIPHERAL_TO_PHYSICAL(x) (x & 0x00FFFFFF)  | (PERIPHERAL_PHYSICAL_BASE & 0xFF000000)
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

#endif

