#ifndef __SYS_H__
#define __SYS_H__

#define SYS_GET_TASK_ID     0

#endif

#ifndef __ASSEMBLY__

#include "stdint.h"

/* Function in sys.S */
extern uint64_t get_taskid();

#endif