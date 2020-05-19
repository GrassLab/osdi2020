#include "typedef.h"

void printf(char *fmt, ...);

// system call
uint64_t get_taskid();
void exit(int status);
uint32_t uart_write(const char buf[], uint32_t size);
