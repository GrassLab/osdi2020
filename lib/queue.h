#ifndef QUEUE
#define QUEUE

#define QUEUE_MAX_SIZE 2048

struct queue {  // circular queue
    int front;
    int rear;
    int size;
    char buf[QUEUE_MAX_SIZE];
};

#endif

#define QUEUE_INIT(q, qsize) \
    q.front = 0;             \
    q.rear = 0;              \
    q.size = qsize

#define QUEUE_EMPTY(q) (q.front == q.rear)

#define QUEUE_FULL(q) (q.front == (q.rear + 1) % q.size)

#define QUEUE_PUSH(q, val)              \
    if (!QUEUE_FULL(q)) {               \
        q.buf[q.rear] = val;            \
        q.rear = (q.rear + 1) % q.size; \
    }

#define QUEUE_POP_ELMT(q) \
    q.buf[q.front];       \
    q.front = (q.front + 1) % q.size

#define QUEUE_POP(q) QUEUE_EMPTY(q) ? '\0' : QUEUE_POP_ELMT(q)
