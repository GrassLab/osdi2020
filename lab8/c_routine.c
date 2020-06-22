#include "syscall.h"

extern int main(void);
void _start(void) __attribute__((section(".text.start")));

void _start(void)
{
  syscall_exit(main());
}

