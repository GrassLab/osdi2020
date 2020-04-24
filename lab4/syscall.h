#include <stdint.h>
#ifndef __SYSCALL_H_
#define __SYSCALL_H_

int syscall_exc(uint64_t ELR_EL1, uint8_t exception_class, uint32_t exception_iss);
int syscall_timer_int(void);

#endif

