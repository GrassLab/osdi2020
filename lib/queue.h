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

void queue_init(struct queue* q, int size);
int queue_empty(struct queue* q);
int queue_full(struct queue* q);
void queue_push(struct queue* q, char val);
char queue_pop(struct queue* q);
