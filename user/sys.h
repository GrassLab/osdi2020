#ifndef __SYS_H__
#define __SYS_H__

#define SYS_GET_TASK_ID     0
#define SYS_UART_READ       1
#define SYS_UART_WRITE      2
#define SYS_EXEC            3
#define SYS_FORK            4
#define SYS_EXIT            5
#define SYS_REMAIN_PAGE     6

#endif

#ifndef __ASSEMBLY__

#include "typedef.h"

/* Function in sys.S */
uint64_t get_taskid();
uint32_t uart_read(char buf[], uint32_t size);
uint32_t uart_write(const char buf[], uint32_t size);
int exec(void(*func)());
int fork();
void exit(int status);
void printf(char* fmt, ...);
uint64_t remain_page_num();

#endif