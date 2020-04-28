#ifndef LOCK_H_
#define LOCK_H_

#include "types.h"

struct mutex {
  uint8_t lock;
};

void mutex_init(struct mutex *lock);
void mutex_lock(struct mutex *lock);
void mutex_unlock(struct mutex *lock);

#endif // LOCK_H_
