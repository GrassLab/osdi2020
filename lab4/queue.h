#include <stdint.h>
#ifndef __QUEUE_H__
#define __QUEUE_H__

/* Original author: JackGrence */

#define QUEUE_MAX_SIZE 0x100

typedef struct _char_queue
{
  char data[QUEUE_MAX_SIZE];
  int head;
  int tail;
} char_queue;

typedef struct _uint64_t_queue
{
  uint64_t data[QUEUE_MAX_SIZE];
  int head;
  int tail;
} uint64_t_queue;

/* WARNING: Take care of macro expansion */
#define QUEUE_INIT(q) q.head = 0; q.tail = 0 /* Required if it is not stored in static (bss) */
#define QUEUE_PUSH(q, val) q.data[q.tail] = val; q.tail = (q.tail + 1) % QUEUE_MAX_SIZE
#define QUEUE_POP(q) q.data[q.head]; q.head = (q.head + 1) % QUEUE_MAX_SIZE
#define QUEUE_EMPTY(q) (q.head == q.tail)
#define QUEUE_FULL(q) ((q.tail + 1) % QUEUE_MAX_SIZE == q.head)

#endif

