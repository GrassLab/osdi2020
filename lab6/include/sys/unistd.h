#ifndef __SYS_UNISTD_H
#define __SYS_UNISTD_H

#include <stdint.h>

void exec(void (*func)());
int fork(void);
void exit(int status);
uint64_t getTaskId(void);

#endif
