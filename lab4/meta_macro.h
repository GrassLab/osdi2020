#ifndef __META_MACRO_H__
#define __META_MACRO_H__

#define UNUSED(x) (void)(x)
#define PERIPHERAL_BUS_BASE 0x7E000000
#define PERIPHERAL_PHYSICAL_BASE 0x3F000000
#define PERIPHERAL_TO_PHYSICAL(x) ((x & 0x00FFFFFF) | (PERIPHERAL_PHYSICAL_BASE & 0xFF000000))
#define CHECK_BIT(var,pos) ((var) & ((unsigned)1<<(pos)))
#define CLEAR_BIT(var, pos) ((var) &= ~((unsigned)1 << (pos)))
#define SET_BIT(var, pos) ((var) |= ((unsigned)1 << (pos)))

#define ANSI_RED       "\x1b[31m"
#define ANSI_GREEN     "\x1b[32m"
#define ANSI_YELLOW    "\x1b[33m"
#define ANSI_BLUE      "\x1b[34m"
#define ANSI_MAGENTA   "\x1b[35m"
#define ANSI_BG_RED    "\x1b[41;1m"
#define ANSI_BG_GREEN  "\x1b[42;1m"
#define ANSI_BG_YELLOW "\x1b[43;1m"
#define ANSI_BG_WHITE  "\x1b[47;1m"
#define ANSI_RESET     "\x1b[0m"

#define NUM_TO_CHAR(x) (char)(x + '0')

#endif

