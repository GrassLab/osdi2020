#include <stdint.h>
#ifndef __QUEUE_H__
#define __QUEUE_H__

/* Original author: JackGrence */

#define QUEUE_MAX_SIZE 0x100
#define QUEUE_TOTAL_PRIORITIES 2u

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

typedef struct _uint64_t_priority_queue
{
  uint64_t_queue q[QUEUE_TOTAL_PRIORITIES];
} uint64_t_pqueue;

/* WARNING: Take care of macro expansion */
#define QUEUE_INIT(q) q.head = 0; q.tail = 0 /* Required if it is not stored in static (bss) */
#define QUEUE_PUSH(q, val) q.data[q.tail] = val; q.tail = (q.tail + 1) % QUEUE_MAX_SIZE
#define QUEUE_POP(q) q.data[q.head]; q.head = (q.head + 1) % QUEUE_MAX_SIZE
#define QUEUE_EMPTY(q) (q.head == q.tail)
#define QUEUE_FULL(q) ((q.tail + 1) % QUEUE_MAX_SIZE == q.head)

void pqueue_uint64_t_init(uint64_t_pqueue * pq);
void pqueue_uint64_t_push(uint64_t_pqueue * pq, unsigned priority, uint64_t val);
uint64_t pqueue_uint64_t_pop(uint64_t_pqueue * pq, unsigned priority);
int pqueue_uint64_t_empty(uint64_t_pqueue * pq, unsigned priority);
int pqueue_uint64_t_full(uint64_t_pqueue * pq, unsigned priority);
int pqueue_uint64_t_size(uint64_t_pqueue * pq, unsigned priority);

#endif

