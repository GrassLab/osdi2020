#include "meta_macro.h"
#include <stdint.h>

#ifndef __PM_H__
#define __PM_H__

#define PM_PASSWORD 0x5A000000

/* Unknown size */
#define PM_RSTC ((uint32_t *)0x3F10001C)
#define PM_WDOG ((uint32_t *)0x3F100024)

#endif

