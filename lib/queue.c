#include "queue.h"

void queue_init(struct queue* q, int max) {
    q->front = 0;
    q->rear = 0;
    q->max = max;
}

int queue_empty(struct queue* q) {
    return q->front == q->rear;
}

int queue_full(struct queue* q) {
    return q->front == (q->rear + 1) % q->max;
}

void queue_push(struct queue* q, char val) {
    if (queue_full(q)) return;  // drop if full
    q->buf[q->rear] = val;
    q->rear = (q->rear + 1) % q->max;
}

char queue_pop(struct queue* q) {
    if (queue_empty(q)) return '\0';
    char elmt = q->buf[q->front];
    q->front = (q->front + 1) % q->max;
    return elmt;
}
