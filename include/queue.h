#define TASK_CAPACITY 64
#define TASK_STACK_SIZE 4096

#define QUEUE_EMPTY(q)      (q.tail == q.head)
#define QUEUE_FULL(q)       ((q.tail + 1) % TASK_CAPACITY == q.head)
#define QUEUE_POP(q)        (q.head = (q.head + 1) % TASK_CAPACITY)
#define QUEUE_PUSH(q)       (q.tail = (q.tail + 1) % TASK_CAPACITY)
#define QUEUE_GET(q)        (q.buf[q.head])
#define QUEUE_SET(q, val)   (q.buf[q.tail] = val)

struct runqueue {
    int head;
    int tail;
    unsigned long buf[TASK_CAPACITY];
};

// struct runqueue RunQueue;

