#ifndef __SYS_UNISTD_H
#define __SYS_UNISTD_H

void exec(void (*func)());
int fork(void);
void exit(int status);

#endif
