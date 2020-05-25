#ifndef KERNEL_LOCK_H_
#define KERNEL_LOCK_H_

#include "kernel/lib/types.h"

struct mutex {
  uint8_t lock;
};

void mutex_init(struct mutex *lock);
void mutex_lock(struct mutex *lock);
void mutex_unlock(struct mutex *lock);

#endif // KERNEL_LOCK_H_
