#include <stddef.h>

#include "queue.h"

void enqueue(struct queue *q, void *ptr) {
  if (q->size >= MAX_QUEUE_SIZE) {
    return;
  }
  q->buf[q->head] = ptr;
  q->head = (q->head + 1) % MAX_QUEUE_SIZE;
  ++(q->size);
}

void *dequeue(struct queue *q) {
  if (q->size <= 0) {
    return NULL;
  }
  void *ptr = q->buf[q->tail];
  q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
  --(q->size);
  return ptr;
}
