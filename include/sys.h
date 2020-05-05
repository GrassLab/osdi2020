#include "stdint.h"

#ifndef __SYS_H__
#define __SYS_H__

#define SYS_GET_CNTFRQ      0
#define SYS_GET_CNTPCT      1

#endif

uint64_t sys_get_cntfrq();
uint64_t sys_get_cntpct();
