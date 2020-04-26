#ifndef __QUEUE_QUEUE_H
#define __QUEUE_QUEUE_H

#include <stddef.h>

typedef struct __Queue {
    void **buffer;
    const size_t capacity;
    size_t front;
    size_t back;
} Queue;

void pushQueue(Queue *self, void *task);
void *popQueue(Queue *self);

#endif
