#ifndef __SYS_H__
#define __SYS_H__

#define SYS_GET_TASK_ID     0
#define SYS_UART_READ       1
#define SYS_UART_WRITE      2
#define SYS_EXEC            3

#endif

#ifndef __ASSEMBLY__

#include "stdint.h"

/* Function in sys.S */
extern uint64_t get_taskid();
extern uint32_t uart_read(char buf[], uint32_t size);
extern uint32_t uart_write(const char buf[], uint32_t size);
extern int exec(void(*func)());

#endif