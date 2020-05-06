#ifndef QUEUE_H_
#define QUEUE_H_

#define MAX_QUEUE_SIZE 64

struct queue {
  void *buf[MAX_QUEUE_SIZE];
  int head;
  int tail;
  int size;
};

void enqueue(struct queue *q, void *ptr);
void *dequeue(struct queue *q);

#endif // QUEUE_H_
