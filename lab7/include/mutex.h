#ifndef __MUTEX_H__
#define __MUTEX_H__

typedef struct mutex_tag {
  int lock;
  unsigned long pid;
} Mutex;

#endif
