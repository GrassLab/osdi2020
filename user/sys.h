#ifndef __SYS_H__
#define __SYS_H__

#define SYS_GET_TASK_ID     0
#define SYS_UART_READ       1
#define SYS_UART_WRITE      2
#define SYS_EXEC            3
#define SYS_FORK            4
#define SYS_EXIT            5
#define SYS_REMAIN_PAGE     6
#define SYS_OPEN            7
#define SYS_CLOSE           8
#define SYS_WRITE           9
#define SYS_READ            10
#define SYS_READDIR         11

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
int open(const char* pathname, int flags);
int close(int fd);
int write(int fd, const void* buf, uint64_t len);
int read(int fd, void* buf, uint64_t len);
int readdir(int fd);

#endif