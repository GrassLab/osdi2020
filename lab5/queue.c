#include "queue.h"

void pqueue_uint64_t_init(uint64_t_pqueue * pq)
{
  for(unsigned i = 0; i < QUEUE_TOTAL_PRIORITIES; ++i)
  {
    QUEUE_INIT(((pq -> q)[i]));
  }
  return;
}

void pqueue_uint64_t_push(uint64_t_pqueue * pq, unsigned priority, uint64_t val)
{
  QUEUE_PUSH(((pq -> q)[priority]), val);
  return;
}

uint64_t pqueue_uint64_t_pop(uint64_t_pqueue * pq, unsigned priority)
{
  uint64_t val = QUEUE_POP(((pq -> q)[priority]));
  return val;
}

int pqueue_uint64_t_empty(uint64_t_pqueue * pq, unsigned priority)
{
  return QUEUE_EMPTY(((pq -> q)[priority]));
}

int pqueue_uint64_t_full(uint64_t_pqueue * pq, unsigned priority)
{
  return QUEUE_FULL(((pq -> q)[priority]));
}

int pqueue_uint64_t_size(uint64_t_pqueue * pq, unsigned priority)
{
  int head = ((pq -> q)[priority]).head;
  int tail = ((pq -> q)[priority]).tail;
  if(tail >= head)
  {
    return tail - head;
  }
  else /* head > tail */
  {
    return QUEUE_MAX_SIZE - head + tail;
  }
}

